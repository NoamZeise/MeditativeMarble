#include "world.h"

#include "model_gen.h"
#include "noise.h"

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
    activePool->tex()->load("textures/test.png");
    gennedModelInfo.meshes.back().diffuseTextures = { "terrian_tex.png"};
    model = activePool->model()->load(gennedModelInfo);
    render->LoadResourcesToGPU(activePool);
}

bool World::checkCollision(glm::vec3 pos) {
    return pos.z < surfaceFn(pos.x, pos.y).z;
}

void World::Update(glm::vec3 playerPos) {
    ///TODO: load/unload map as player moves around
}

void World::Draw(Render* render) {
    render->DrawModel(
	    model,
	    glm::mat4(1.0f),
	    glm::mat4(1.0f));
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
