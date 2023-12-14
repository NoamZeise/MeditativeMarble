#include "physics.h"

void PhysObj::setAcceleration(glm::vec3 acceleration) {
    this->acceleration = acceleration;
}

void PhysObj::addAcceleration(glm::vec3 acceleration) {
    this->acceleration += acceleration;
}

void PhysObj::addVelocity(glm::vec3 velocity) { this->velocity += velocity; }


#include <graphics/logger.h>

void print_vec3(glm::vec3 v, std::string name) {
    LOG(name << ".x: " << v.x << "  " <<
	name << ".y: " << v.y << "  " <<
	name << ".z: " << v.z);
}

void Sphere::worldCollision(World *world) {
    grounded = false;
    glm::vec3 np = world->nearestPoint(pos);
    glm::vec3 dir = pos - np;
    if(dir != glm::vec3(0)) {
	collisionN = glm::normalize(dir);
	glm::vec3 side = glm::cross(velocity, -collisionN);
	collisionT = glm::cross(-collisionN, side);
    }
    bool insideShape = world->checkCollision(pos);
    if(insideShape || glm::dot(dir, dir) < radius*radius) {
	grounded = true;
	if(dir != glm::vec3(0)) {
	    collisionN = glm::normalize(dir);
	}
	if(insideShape) { // always pointing away from surface even if inside shape
	    collisionN *= -1;
	}
	// push sphere out of surface
	pos += radius*collisionN - dir;

	//bounce
	glm::vec3 bounce = glm::dot(-collisionN, velocity)*collisionN;
	velocity += bounce*bounceCoeff;
	
	//friction
	glm::vec3 side = glm::cross(velocity, -collisionN);
	if(side != glm::vec3(0)) {
	    side = glm::normalize(side);
	}
	collisionT = glm::cross(-collisionN, side);
	float tangentSpeed = glm::dot(collisionT, velocity);
	glm::vec3 friction = tangentSpeed*collisionT;
	velocity -= friction*frictionCoeff;

	//spin
	glm::vec3 BN = glm::cross(collisionT, collisionN);
	spinAxis = 0.5f*(spinAxis + frictionCoeff*tangentSpeed*BN);
    }
}

void Sphere::Update(long long dt) {
    if(isGrounded()) {
	glm::vec3 spinDir = glm::cross(collisionN, spinAxis);
	float speed = glm::dot(spinDir, velocity);
	float spinSpeed = glm::length(spinDir);
	float diff = spinSpeed - speed;
	if(spinSpeed > speed) {
	    velocity += (float)dt*diff*spinDir;
	} else if(spinSpeed != 0) {
	    spinDir *= 1 - (0.1f*dt)*diff;
	}
    } else {
	spinAxis *= 1 - (0.01f*dt);
    }
    PhysObj::Update(dt);
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
