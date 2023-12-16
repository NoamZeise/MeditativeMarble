#ifndef WORLD_H
#define WORLD_H

#include <graphics/render.h>
#include <graphics/model_loader.h>

#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <chrono>

struct Chunk {
    Resource::Model model;
    bool inGpu = false;
    Resource::Pool pool;
};

struct BufferedChunk {
    ModelInfo::Model model;
    glm::vec4 rect;
};

struct Buffered {
    BufferedChunk main;
    BufferedChunk midLod[4];
    BufferedChunk lowLod[4];
    bool loaded = false;
};

class World {
 public:
    World(Render* render, bool multithreadPools);
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
    int bestChunk(glm::vec2 pos, float *bestReturn);

    bool recreate = false;
    ResourcePool* activePool;
    ResourcePool* inactivePool;
    Render* render;

    std::vector<Chunk> chunks;
    glm::vec4 mainRect;
    static const int BC_SIZE = 10;
    Buffered bufferedChunks[BC_SIZE];
    int currentBc = 0;
    std::mutex usingCurrentBc;

    bool threadActive = false;
    std::atomic<bool> loadingFinished;
    std::thread loadingThread;

    bool loadingChunk = false;
    std::thread useChunkThread;
    std::atomic<bool> useChunkLoaded = false;
    float loadTime = 0.0f;
    
    bool multithreadPools;
    std::function<glm::vec3(float, float)> surfaceFn;

    std::chrono::time_point<std::chrono::system_clock> startTime;
};

#endif /* WORLD_H */
