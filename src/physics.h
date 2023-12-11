#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>

class PhysObj {
public:
    void Update(long long dt);
    void setAcceleration(glm::vec3 acceleration);
    glm::vec3 getPos() {
	return pos;
    }
    void fixPos() {
	pos = prevPos;
	velocity.z = 0;
    }
 private:
    glm::vec3 pos = glm::vec3(0);
    glm::vec3 prevPos = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 acceleration = glm::vec3(0);
};

class Sphere : public PhysObj {
public:
    float radius;
};

#endif /* PHYSICS_H */
