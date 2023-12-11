#include "physics.h"

void PhysObj::setAcceleration(glm::vec3 acceleration) {
    this->acceleration = acceleration;
}

void PhysObj::addAcceleration(glm::vec3 acceleration) {
    this->acceleration += acceleration;
}

void Sphere::worldCollision(World *world) {
    glm::vec3 np = world->nearestPoint(pos);
    glm::vec3 dir = np - pos;
    bool insideShape = world->checkCollision(pos);
    if(insideShape || glm::dot(dir, dir) < radius*radius) {
	glm::vec3 toSurf = glm::normalize(dir);
	glm::vec3 change = toSurf;
	if(!insideShape) {
	    change *= -1;
	}
	pos += dir + change;
	glm::vec3 bounce = glm::dot(-change, velocity)*change;
	velocity += bounce;
    }
}

void PhysObj::Update(long long dt) {
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


/// --- Physics Manager ---

PhysicsManager::PhysicsManager(World *world) {
    this->globalAcceleration = glm::vec3(0, 0, -0.00005);
    this->world = world;
}

void PhysicsManager::addPhysObj(PhysObj *obj) {
    objs.push_back(obj);
}

void PhysicsManager::Update(long long dt) {
    if(dt > 100)
	return;
    // obj vs world collisions
    for(auto obj: objs) {
	obj->addAcceleration(globalAcceleration);
	obj->Update(dt);
	obj->worldCollision(world);
    }
}
