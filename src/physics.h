#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>
#include "world.h"

class PhysObj {
public:
    virtual void Update(long long dt);
    void setAcceleration(glm::vec3 acceleration);
    void addAcceleration(glm::vec3 acceleration);
    void addVelocity(glm::vec3 velocity);
    glm::vec3 getVel() { return velocity; }
    glm::vec3 getPos() {
	return pos;
    }
    virtual void worldCollision(World* world) = 0;
    bool isGrounded() { return grounded; }
    bool hasGlobalAcceleration = true;
    float frictionCoeff = 0.002f;
    float bounceCoeff = 1.50f;
 protected:
    glm::vec3 pos = glm::vec3(0);
    glm::vec3 prevPos = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 acceleration = glm::vec3(0);

    /// collision
    bool grounded = true;
    glm::vec3 collisionN;
    glm::vec3 collisionT;
};

class Sphere : public PhysObj {
public:
    void worldCollision(World* world) override;
    void Update(long long dt) override;
    float radius;
protected:
    glm::vec3 spinAxis = glm::vec3(0);
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
