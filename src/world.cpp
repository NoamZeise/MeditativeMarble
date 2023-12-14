#include "world.h"

#include "model_gen.h"
#include "noise.h"
#include <graphics/logger.h>

glm::vec3 surfaceFn(float x, float y) {
    float i[] = {x*0.004f, y*0.004f, 0};
    return glm::vec3(x, y, (noise::simplex<3>(i) * 60 - 100));
    //return glm::vec3(x, y, -5);
    //return glm::vec3(x, y, (sin(x*0.01) + cos(y*0.01))*100 - 120);
    //return glm::vec3(x, y, 50.0f*(sin(0.02f*x) - cos(0.02f*y)) - 80);
    //return glm::vec3(x, y, -pow((cos(x*0.01)*cos(x*0.01) + cos(y*0.01)*cos(y*0.01)), 2) * 50 - 100);
}


World::World(Render *render) {
    this->render = render;
    activePool = render->CreateResourcePool();
    inactivePool = render->CreateResourcePool();
    
    ModelInfo::Model gennedModelInfo = genSurface(
	    surfaceFn, true, 10.0f,
	    {-500.0f, 500.0f, 5.0f},
	    {-500.0f, 500.0f, 5.0f});
    //gennedModelInfo.meshes.back().diffuseColour = glm::vec4(0.3, 0.5, 0.2, 1);
    gennedModelInfo.meshes.back().diffuseTextures = { "terrian_tex.png"};
    loadChunkAtPoint(glm::vec3(0));
    loadChunksToGPU();
}

void World::loadChunksToGPU() {
    auto t = activePool;
    activePool = inactivePool;
    inactivePool = t;
    render->LoadResourcesToGPU(activePool);
    for(int i = 0; i < chunks.size(); i++) {
	if(!chunks[i].inGpu) {
	    chunks[i].inGpu = true;
	}
	else if(!chunks[i].inInactive) {
	    chunks[i].inInactive = true;
	}
	else
	    chunks.erase(chunks.begin() + i--);
    }
}

bool World::checkCollision(glm::vec3 pos) {
    return pos.z < surfaceFn(pos.x, pos.y).z;
}

const glm::vec2 CHUNK_SIZE(1000, 1000);
const float SUB_CHUNK_SIZE = 1500.0f;
const float MAIN_CHUNK_RESOLUTION = 5.0f;
const float SUB_CHUNK_RESOLUTION = 50.0f;
const float LOD_OVERLAP = 15.0f;
const float UV_DENSITY = 10.0f;

void World::loadChunkAtPoint(glm::vec3 pos) {
    glm::vec4 chunkRect = glm::vec4(pos.x - CHUNK_SIZE.x/2,
				    pos.y - CHUNK_SIZE.y/2,
				    CHUNK_SIZE.x, CHUNK_SIZE.y);
    ModelInfo::Model mainChunk = genSurface(
	    surfaceFn, true, UV_DENSITY,
	    {chunkRect.x, chunkRect.z + chunkRect.x, MAIN_CHUNK_RESOLUTION},
	    {chunkRect.y, chunkRect.w + chunkRect.y, MAIN_CHUNK_RESOLUTION});
    mainChunk.meshes.back().diffuseTextures = { "terrian_tex.png"};
    chunks.push_back({
	    inactivePool->model()->load(mainChunk),
	});
    ModelInfo::Model subChunks[4];
    for(int i = 0; i < 4; i++) {
	glm::vec4 subChunkRect = chunkRect;
	switch(i) {
	case 0: //top left
	    subChunkRect.x -= SUB_CHUNK_SIZE;
	    subChunkRect.y -= SUB_CHUNK_SIZE;
	    subChunkRect.z = CHUNK_SIZE.x + 2*SUB_CHUNK_SIZE;
	    subChunkRect.w = SUB_CHUNK_SIZE;
	    break;
	case 1:	//left
	    subChunkRect.x -= SUB_CHUNK_SIZE;
	    subChunkRect.z = SUB_CHUNK_SIZE;
	    subChunkRect.w = CHUNK_SIZE.y;
	    break;
	case 2: //right
	    subChunkRect.x += CHUNK_SIZE.x;
	    subChunkRect.z = SUB_CHUNK_SIZE;
	    subChunkRect.w = CHUNK_SIZE.y;
	    break;
	case 3: //bottom left
	    subChunkRect.x -= SUB_CHUNK_SIZE;
	    subChunkRect.y += CHUNK_SIZE.y;
	    subChunkRect.z = CHUNK_SIZE.x + 2*SUB_CHUNK_SIZE;
	    subChunkRect.w = SUB_CHUNK_SIZE;
	    break;
	}
	subChunkRect.x -= LOD_OVERLAP;
	subChunkRect.y -= LOD_OVERLAP;
	subChunkRect.z += LOD_OVERLAP;
	subChunkRect.w += LOD_OVERLAP;
	subChunks[i] = genSurface(
		[](float a, float b){
		    auto v = surfaceFn(a, b);
		    v.z -= 4.0f;
		    return v;},
		true, UV_DENSITY,
		{subChunkRect.x, subChunkRect.z + subChunkRect.x, SUB_CHUNK_RESOLUTION},
		{subChunkRect.y, subChunkRect.w + subChunkRect.y, SUB_CHUNK_RESOLUTION});
	subChunks[i].meshes.back().diffuseTextures = { "terrian_tex.png"};
	chunks.push_back({
		inactivePool->model()->load(subChunks[i]),
	    });
    }
    loadBarrier = chunkRect;
    loadBarrier.x += CHUNK_SIZE.x /4;
    loadBarrier.y += CHUNK_SIZE.y /4;
    loadBarrier.z = CHUNK_SIZE.x /2;
    loadBarrier.w = CHUNK_SIZE.y /2;
}

void World::Update(glm::vec3 playerPos) {
    if(threadActive) {
	if(loadingFinished) {
	    loadingThread.join();
	    loadChunksToGPU();
	    recreate = true;
	    threadActive = false;
	} else {
	    return;
	}
    }
    if(loadBarrier.x > playerPos.x || loadBarrier.x + loadBarrier.z < playerPos.x
       || loadBarrier.y > playerPos.y || loadBarrier.y + loadBarrier.w < playerPos.y) {
	threadActive = true;
	loadingFinished = false;
	loadingThread = std::thread([this, playerPos] {
	    loadChunkAtPoint(playerPos);
	    loadingFinished = true;
	});
    }
}

void World::Draw(Render* render) {
    for(auto &c: chunks) {
	if(c.inGpu && !c.inInactive) {
	    render->DrawModel(
		    c.model,
		    glm::mat4(1.0f),
		    glm::mat4(1.0f));
	}
    }
}

struct fnArgs {float a; float b; };

fnArgs getMin(float a, float b, std::function<float (float, float)> f) {
    float h = 0.1f;
    float step = 0.1f;
    const int ITERS = 10;
    for(int i = 0; i < ITERS; i++) {
	// calc del(d2(a, b))
	float ra = f(a + h, b);
	float rb = f(a, b + h);
	float ram = f(a - h, b);
	float rbm = f(a, b - h);
	float da = (ra - ram)/(2*h);
	float db = (rb - rbm)/(2*h);
	a -= step*da;
	b -= step*db;
    }
    return {a, b};
}

glm::vec3 World::nearestPoint(glm::vec3 pos) {
    std::function<glm::vec3 (float, float)> fn = surfaceFn;
    //dist squared.
    std::function<float (float, float)> d2 = [&pos, &fn](float a, float b) {
	glm::vec3 d = pos - fn(a, b);
	return glm::dot(d, d);
    };
    fnArgs g = getMin(pos.x, pos.y, d2);
    return surfaceFn(g.a, g.b);
}

float World::nearestPointToEnd(glm::vec3 start, glm::vec3 end) {
    std::function<glm::vec3 (float, float)> fn = surfaceFn;
    std::function<float(float)> d2 = [&start, &end, &fn](float t) {
	glm::vec3 v = end + t*(start - end);
	glm::vec3 s = fn(v.x, v.y);
	return fabs(v.z - s.z);
    };
    float t = 0;
    float h = 0.005f;
    float step = 0.01f;
    const int ITERS = 10;
    for(int i = 0; i < ITERS; i++) {
	float r = d2(t);
	float ra = d2(t + h);
	float fp = (ra - r)/h;
	t-=step*fp;
	if(t < 0)
	    t += 0.5*step*fp;
    }
    if(t < 0) t = 0;
    if(t > 1) t = 1;
    return t;
}
