#include "world.h"

#include "model_gen.h"
#include "noise.h"

glm::vec3 surfaceFn(float x, float y) {
    float i[] = {x*0.004f, y*0.004f, 0};
    return glm::vec3(x, y, (noise::simplex<3>(i) * 80 - 100));
}


World::World(ModelLoader *loader) {
    ModelInfo::Model gennedModelInfo = genSurface(
	    surfaceFn, false,
	    {-1000.0f, 1000.0f, 4.0f},
	    {-1000.0f, 1000.0f, 4.0f});
    gennedModelInfo.meshes.back().diffuseColour = glm::vec4(0.3, 0.7, 0.4, 1);
    model = loader->load(gennedModelInfo);
}

bool World::checkCollision(glm::vec3 pos) {
    return pos.z < surfaceFn(pos.x, pos.y).z;
}

void World::Draw(Render* render) {
    render->DrawModel(
	    model,
	    glm::mat4(1.0f),
	    glm::mat4(1.0f));
}

glm::vec3 World::nearestPoint(glm::vec3 pos) {
    std::function<glm::vec3 (float, float)> fn = surfaceFn;
    //dist squared.
    std::function<float (float, float)> d2 = [&pos, &fn](float a, float b) {
	glm::vec3 d = pos - fn(a, b);
	return glm::dot(d, d);
    };

    float a = pos.x; // intial guess for a,b
    float b = pos.y;
    float h = 0.1f;
    float step = 0.1f;
    const int ITERS = 20;
    for(int i = 0; i < ITERS; i++) {
	// calc del(d2(a, b))
	float r = d2(a, b);
	float ra = d2(a + h, b);
	float rb = d2(a, b + h);
	float da = (ra - r)/h;
	float db = (rb - r)/h;
	a -= step*da;
	b -= step*db;
    }

    return fn(a, b);
}
