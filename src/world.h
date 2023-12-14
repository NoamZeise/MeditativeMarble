#ifndef WORLD_H
#define WORLD_H

#include <graphics/render.h>
#include <graphics/model_loader.h>

#include <vector>
#include <thread>

struct Chunk {
    Resource::Model model;
    bool inGpu = false;
    bool inInactive = false;
};

class World {
 public:
    World(Render* render);
    bool checkCollision(glm::vec3 pos);
    glm::vec3 nearestPoint(glm::vec3 pos);
    float nearestPointToEnd(glm::vec3 start, glm::vec3 end);
    void Update(glm::vec3 playerPos);
    void Draw(Render* render);
    bool recreationRequired() {
	bool r = recreate;
	recreate = false;
	return r;
    }
 private:

    void loadChunkAtPoint(glm::vec3 pos);
    void loadChunksToGPU();

    bool recreate = false;
    ResourcePool* activePool;
    ResourcePool* inactivePool;
    Render* render;
    glm::vec4 mainChunk;
    glm::vec4 loadBarrier;
    std::vector<Chunk> chunks;

    bool threadActive = false;
    std::atomic<bool> loadingFinished;
    std::thread loadingThread;
};

#endif /* WORLD_H */
