#include "world.h"

#include "model_gen.h"
#include "noise.h"

glm::vec3 surfaceFn(float x, float y) {
    float i[] = {x*0.008f, y*0.008f, 0};
    return glm::vec3(x, y, noise::simplex<3>(i) * 15 - 20);
}


World::World(ModelLoader *loader) {
    ModelInfo::Model gennedModelInfo = genSurface(
	    surfaceFn, true,
	    {-500.0f, 500.0f, 4.0f},
	    {-500.0f, 500.0f, 4.0f});
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
