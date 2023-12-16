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
    bool insideShape = world->checkCollision(pos);
    if(insideShape || glm::dot(dir, dir) < radius*radius) {
	collisionN = glm::normalize(dir);
	glm::vec3 side = glm::cross(velocity, -collisionN);
	collisionT = glm::cross(-collisionN, side);
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
	spinAxis *= 1 - (0.001f*dt);
    }
    PhysObj::Update(dt);
}

void PhysObj::Update(long long dt) {
    prevPos = pos;
    velocity += glm::vec3(
	    acceleration.x * dt,
	    acceleration.y * dt,
	    acceleration.z * dt);
    if(glm::dot(velocity, velocity) > terminalVel)
	velocity *=0.99;
    pos += glm::vec3(
	    velocity.x * dt,
	    velocity.y * dt,
	    velocity.z * dt);
}


/// --- Physics Manager ---

PhysicsManager::PhysicsManager(World *world) {
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

glm::vec3 PhysicsManager::fixCamPos(glm::vec3 localCamPos, float *camRad, glm::vec3 targetPos, glm::vec3 targetVel) {
    targetVel.z = 0;
    float mod = glm::length(targetVel)*100.0f;
    if(mod < INITAL_CAM_RAD)
	mod = INITAL_CAM_RAD;
    float change = *camRad - mod;
    *camRad = (*camRad + mod)*0.5f;
    if(glm::dot(targetVel, targetVel) > 0.1)
	targetVel.z = -0.01;
    glm::vec3 cp = -targetVel + localCamPos;
    if(cp != glm::vec3(0)) {
	cp = glm::normalize(cp);
	glm::vec3 camPos = cp * *camRad + targetPos;
	glm::vec3 np = world->nearestPoint(camPos);
	glm::vec3 n = camPos - np;
	float d = n.z;
	const float CAM_FROM_SURFACE = 4.0f;
	if(d < CAM_FROM_SURFACE && n != glm::vec3(0)) {
	    glm::normalize(n);
	    if(d < 0) {
		n *= -1.0f;
		d = 0;
	    }
	    glm::vec3 newWorldPos = camPos
		+ (float)(fabs(d - CAM_FROM_SURFACE))*n;
	    float t = (CAM_FROM_SURFACE - d)/CAM_FROM_SURFACE;
	    glm::vec3 mid = camPos +
		(t)*(newWorldPos - camPos);
	    cp = (1 / *camRad)*(mid - targetPos);
	    if(cp != glm::vec3(0))
		cp = glm::normalize(cp);
	    else
		cp = glm::vec3(0, 0, 1);
	}	        
    } else {
	cp = glm::vec3(0, 0, 1);
    }
    return cp;
}
