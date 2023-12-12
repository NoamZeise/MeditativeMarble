#include "physics.h"

void PhysObj::setAcceleration(glm::vec3 acceleration) {
    this->acceleration = acceleration;
}

void PhysObj::addAcceleration(glm::vec3 acceleration) {
    this->acceleration += acceleration;
}

void PhysObj::addVelocity(glm::vec3 velocity) {
    this->velocity += velocity;
}

void Sphere::worldCollision(World *world) {
    grounded = false;
    glm::vec3 np = world->nearestPoint(pos);
    glm::vec3 dir = pos - np;
    bool insideShape = world->checkCollision(pos);
    addSpin = glm::vec3(0);
    if(insideShape || glm::dot(dir, dir) < radius*radius) {
	grounded = true;
	collisionN = glm::normalize(dir);
	if(insideShape) { // always pointing away from surface even if inside shape
	    collisionN *= -1;
	}
	// push sphere out of surface
	pos += radius*collisionN - dir;

	//bounce
	glm::vec3 bounce = glm::dot(-collisionN, velocity)*collisionN;
	velocity += bounce*bounceCoeff;

	//friction
	glm::vec3 side = glm::normalize(glm::cross(velocity, -collisionN));
	collisionT = glm::cross(-collisionN, side); // mag = 1
	float tangentSpeed = glm::dot(collisionT, velocity);
	glm::vec3 friction = tangentSpeed*collisionT;
	velocity -= friction*frictionCoeff;

	//spin
	spinAxis += 0.01f*tangentSpeed*glm::cross(collisionT, collisionN);
	glm::vec3 spinDir = glm::cross(collisionN, spinAxis);
	float spinSpeed = glm::length(spinDir);
	float speedInDirOfSpin = glm::dot(spinDir, velocity);
	addSpin = 0.1f*(spinSpeed - speedInDirOfSpin)*glm::cross(collisionN, spinAxis);
    }
}

void Sphere::Update(long long dt) {
    if(grounded) {
	velocity += (float)dt*addSpin*0.01f;
    } else {
	spinAxis *= 0.99f;
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
