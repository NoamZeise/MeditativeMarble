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
    World(Render* render);
    bool checkCollision(glm::vec3 pos);
    glm::vec3 nearestPoint(glm::vec3 pos);
    float nearestPointToEnd(glm::vec3 start, glm::vec3 end);
    void Update(glm::vec3 playerPos);
    void Draw(Render* render);
 private:
    Resource::Model model;
    ResourcePool* activePool;
    ResourcePool* inactivePool;
    Render* render;
    std::vector<Chunk> chunks;
};

#endif /* WORLD_H */
