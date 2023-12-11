#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>
#include "world.h"

class PhysObj {
public:
    void Update(long long dt);
    void setAcceleration(glm::vec3 acceleration);
    void addAcceleration(glm::vec3 acceleration);
    glm::vec3 getVel() { return velocity; }
    glm::vec3 getPos() {
	return pos;
    }
    virtual void worldCollision(World* world) = 0;
    bool hasGlobalAcceleration = true;
 protected:
    glm::vec3 pos = glm::vec3(0);
    glm::vec3 prevPos = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 acceleration = glm::vec3(0);
};

class Sphere : public PhysObj {
public:
    void worldCollision(World* world) override;
    float radius;
};

class PhysicsManager  {
public:
    PhysicsManager(World *world);
    void Update(long long dt);
    void addPhysObj(PhysObj* obj);
private:
    World* world;
    std::vector<PhysObj*> objs;
    glm::vec3 globalAcceleration;
};

#endif /* PHYSICS_H */
