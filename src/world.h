#ifndef WORLD_H
#define WORLD_H

#include <graphics/render.h>
#include <graphics/model_loader.h>

#include <vector>

struct Chunk {
    Resource::Model model;
    ModelInfo::Model data;
    glm::vec4 rect;
};

class World {
 public:
    World(ModelLoader *loader);
    bool checkCollision(glm::vec3 pos);
    glm::vec3 nearestPoint(glm::vec3 pos);
    void Update(ResourcePool* pool, glm::vec3 playerPos);
    void Draw(Render* render);
 private:
    Resource::Model model;
    std::vector<Chunk> chunks;
};

#endif /* WORLD_H */
