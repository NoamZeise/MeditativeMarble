#ifndef WORLD_H
#define WORLD_H

#include <graphics/render.h>
#include <graphics/model_loader.h>

class World {
 public:
    World(ModelLoader *loader);
    bool checkCollision(glm::vec3 pos);
    void Draw(Render* render);
 private:
    Resource::Model model;
};

#endif /* WORLD_H */
