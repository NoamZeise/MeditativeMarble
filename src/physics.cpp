#include "physics.h"

void PhysObj::setAcceleration(glm::vec3 acceleration) {
    this->acceleration = acceleration;
}

void PhysObj::fixPos(glm::vec3 collisionPoint) {
    glm::vec3 collisionVec = collisionPoint - pos;
    velocity.z = 0;
    pos = prevPos;
}

void PhysObj::Update(long long dt) {
    if(dt > 100)
	return;
    prevPos = pos;
    velocity += glm::vec3(
	    acceleration.x * dt,
	    acceleration.y * dt,
	    acceleration.z * dt);
    pos += glm::vec3(
	    velocity.x * dt,
	    velocity.y * dt,
	    velocity.z * dt);
}
