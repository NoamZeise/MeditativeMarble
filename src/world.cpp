#include "world.h"

#include "model_gen.h"
#include "noise.h"
#include <graphics/logger.h>
#include <chrono>

glm::vec3 surfaceFn(float x, float y) {
    float i[] = {x*0.003f, y*0.003f, 0};
    float i2[] = {x*0.0005f, y*0.0005f, 10};
    return glm::vec3(x, y, (noise::simplex<3>(i) * 50 +
			    noise::simplex<3>(i2)*200) - 300);
}


World::World(Render *render, bool multithreadPools) {
    this->render = render;
    this->multithreadPools = multithreadPools;
    activePool = render->CreateResourcePool();
    inactivePool = render->CreateResourcePool();
    auto start = std::chrono::high_resolution_clock::now();
    createBuffered(glm::vec3(0));
    useBuffered(bufferedChunks[0]);
    loadChunksToGPU();
    switchPools();
    loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(
	    std::chrono::high_resolution_clock::now() - start).count();
}

World::~World() {
    if(threadActive)
	loadingThread.join();
}

void World::loadChunksToGPU() {
    render->LoadResourcesToGPU(inactivePool);
}

void World::switchPools() {
    render->setResourcePoolInUse(activePool->id(), false);
    render->setResourcePoolInUse(inactivePool->id(), true);
    ResourcePool* temp = activePool;
    activePool = inactivePool;
    inactivePool = temp;

    for(int i = 0; i < chunks.size(); i++) {
	if(!chunks[i].inGpu) {
	    chunks[i].inGpu = true;
	    chunks[i].pool = activePool->id();
	} else
	    chunks.erase(chunks.begin() + i--);
    }
}

bool World::checkCollision(glm::vec3 pos) {
    return pos.z < surfaceFn(pos.x, pos.y).z;
}

const glm::vec2 CHUNK_SIZE(3000, 3000);
const float SUB_CHUNK_SIZE = 1000.0f;
const float MAIN_CHUNK_RESOLUTION = 10.0f;
const float SUB_CHUNK_RESOLUTION = 50.0f;
const float LOD_OVERLAP = 15.0f;
const float UV_DENSITY = 10.0f;

Buffered World::loadBufferedAtPoint(glm::vec3 pos) {
    Buffered b;
    LOG("loading chunk at x: " << pos.x << " y: " << pos.y);
    glm::vec4 chunkRect = glm::vec4(pos.x - CHUNK_SIZE.x/2,
				    pos.y - CHUNK_SIZE.y/2,
				    CHUNK_SIZE.x, CHUNK_SIZE.y);
    b.main.model = genSurface(
	    surfaceFn, true, UV_DENSITY,
	    {chunkRect.x, chunkRect.z + chunkRect.x, MAIN_CHUNK_RESOLUTION},
	    {chunkRect.y, chunkRect.w + chunkRect.y, MAIN_CHUNK_RESOLUTION});
    b.main.model.meshes.back().diffuseTextures = { "terrian_tex.png"};
    b.main.rect = chunkRect;
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
	b.lowLod[i].model = genSurface(
		[](float a, float b){
		    auto v = surfaceFn(a, b);
		    v.z -= 4.0f;
		    return v;},
		true, UV_DENSITY,
		{subChunkRect.x, subChunkRect.z + subChunkRect.x, SUB_CHUNK_RESOLUTION},
		{subChunkRect.y, subChunkRect.w + subChunkRect.y, SUB_CHUNK_RESOLUTION});
	b.lowLod[i].model.meshes.back().diffuseTextures = { "terrian_tex.png"};
	b.lowLod[i].rect = subChunkRect;
    }
    b.loaded = true;
    return b;
}

void World::createBuffered(glm::vec3 pos) {
    bufferedChunks[currentBc] = loadBufferedAtPoint(pos);
    usingCurrentBc.lock();
    currentBc = (currentBc + 1) % BC_SIZE;
    usingCurrentBc.unlock();
}

void World::useBuffered(Buffered b) {
    mainRect = b.main.rect;
    chunks.push_back({inactivePool->model()->load(b.main.model)});
    for(int i = 0; i < 4; i++) {
	chunks.push_back({inactivePool->model()->load(b.lowLod[i].model)});
    }
}

int World::bestChunk(glm::vec2 pos, float *bestReturn) {
    int bestI = -1;
    float best = -1;
    for(int i = 0; i < BC_SIZE; i++) {
	if(i == currentBc)
	    continue;
	if(bufferedChunks[i].loaded) {
	    glm::vec2 diff =
		glm::vec2(pos.x, pos.y) -
		glm::vec2(bufferedChunks[i].main.rect.x +
			  bufferedChunks[i].main.rect.z/2,
			  bufferedChunks[i].main.rect.y +
			  bufferedChunks[i].main.rect.w/2);
	    float d = glm::dot(diff, diff);
	    if(best == -1 || d < best) {
		best = d;
		bestI = i;
	    }
	}
    }
    if(bestReturn != nullptr)
	*bestReturn = best;
    return bestI;
}

const float FUTURE_CHUNK_TIME = 2000.0f;
const float NEW_CHUNK_CUTOFF = pow(500, 2);

void World::Update(glm::vec3 playerPos, glm::vec3 playerVel) {
    glm::vec3 pos = playerPos + playerVel*FUTURE_CHUNK_TIME;
    float bestChunkDist = -1;
    usingCurrentBc.lock();
    int bestChunkI = bestChunk(pos, &bestChunkDist);
    if(threadActive) {
	if(loadingFinished) {
	    loadingThread.join();
	    threadActive = false;
	} 
    } else {
	if(bestChunkI == -1 || bestChunkDist > NEW_CHUNK_CUTOFF) {
	    threadActive = true;
	    loadingFinished = false;
	    loadingThread = std::thread([this, pos] {
		auto start = std::chrono::high_resolution_clock::now();
		createBuffered(pos);
		loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now() - start).count();
		loadingFinished = true;
	    });
	}
    }
    if(loadingChunk && !recreate) {
	if(useChunkLoaded) {
	    useChunkThread.join();
	    useChunkLoaded = false;
	    if(!multithreadPools)
		loadChunksToGPU();
	    switchPools();
	    loadingChunk = false;
	    recreate = true;
	}
    } else if(!recreate){
	if(playerPos.x < mainRect.x + CHUNK_SIZE.x/4 ||
	   playerPos.y < mainRect.y + CHUNK_SIZE.y/4 ||
	   playerPos.x > mainRect.x + mainRect.z - CHUNK_SIZE.x/4 ||
	   playerPos.y > mainRect.y + mainRect.w - CHUNK_SIZE.y/4) {
	    if(bestChunkI != -1) {
		loadingChunk = true;
		useChunkThread = std::thread([this, b = bufferedChunks[bestChunkI]]{
		    useBuffered(b);
		    if(multithreadPools)
			loadChunksToGPU();
		    useChunkLoaded = true;
		});
	    }
	}
    }
    usingCurrentBc.unlock();
}

void World::Draw(Render* render) {
    for(auto &c: chunks) {
	if(c.inGpu && c.pool.ID == activePool->id().ID) {
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
