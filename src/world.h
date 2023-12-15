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
    Resource::Pool pool;
};

struct BufferedChunk {
    ModelInfo::Model model;
    glm::vec4 rect;
};

struct Buffered {
    BufferedChunk main;
    BufferedChunk lowLod[4];
    bool loaded = false;
};

class World {
 public:
    World(Render* render);
    ~World();
    bool checkCollision(glm::vec3 pos);
    glm::vec3 nearestPoint(glm::vec3 pos);
    float nearestPointToEnd(glm::vec3 start, glm::vec3 end);
    void Update(glm::vec3 playerPos, glm::vec3 playerVel);
    void Draw(Render* render);
    bool recreationRequired() {
	bool r = recreate;
	recreate = false;
	return r;
    }
 private:
    void createBuffered(glm::vec3 pos);
    Buffered loadBufferedAtPoint(glm::vec3 pos);
    void useBuffered(Buffered b);
    void loadChunksToGPU();
    void switchPools();

    bool recreate = false;
    ResourcePool* activePool;
    ResourcePool* inactivePool;
    Render* render;

    std::vector<Chunk> chunks;
    glm::vec4 mainRect;
    static const int BC_SIZE = 10;
    Buffered bufferedChunks[BC_SIZE];
    int currentBc = 0;

    bool threadActive = false;
    std::atomic<bool> loadingFinished;
    std::thread loadingThread;

    bool loadingChunk = false;
    std::thread useChunkThread;
    std::atomic<bool> useChunkLoaded = false;
    float loadTime = 0.0f;
    float loadTimer = 0.0f;
};

#endif /* WORLD_H */
