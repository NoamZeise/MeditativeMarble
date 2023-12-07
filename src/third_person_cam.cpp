#include "third_person_cam.h"
#include <glm/gtc/quaternion.hpp>

ThirdPersonCam::ThirdPersonCam() {
    pos = glm::vec3(1, 0, 0);
    worldPos = pos;
    updateView();
}

void ThirdPersonCam::setWorldUp(glm::vec3 worldUp) {
    this->worldUp = glm::normalize(worldUp);
    updateView();
}

void ThirdPersonCam::setTarget(glm::vec3 target, float radius) {
    this->target = target;
    this->radius = radius;
    targetMat = glm::translate(glm::mat4(1.0f), -target);
    this->worldPos = target + (pos * radius);
    updateView();
}

void ThirdPersonCam::control(gamehelper::Input &input, double dt) {
    glm::vec2 ctrlDir(-input.m.dx(), -input.m.dy());
    ctrlDir *= mouseSensitivity;
    if(ctrlDir.x != 0 || ctrlDir.y != 0) {
	float updot = glm::dot(forward, worldUp);
	if(updot > lim && ctrlDir.y < 0 || -updot > lim && ctrlDir.y > 0)
	    ctrlDir.y = 0;
	float size = 0.001 * dt;
	ctrlDir *= size/2;
	auto qx = glm::quat(cos(ctrlDir.x), (float)sin(ctrlDir.x)*up);
	auto qy = glm::quat(cos(ctrlDir.y), (float)sin(ctrlDir.y)*left);
	auto q = qx * qy;
	auto c = glm::conjugate(q);
	pos = q * pos * c;
    }
    updateView();
}

void ThirdPersonCam::updateView() {
    targetForward = glm::normalize(glm::dot(pos, worldUp)*worldUp - pos);
    targetLeft = glm::normalize(glm::cross(worldUp, targetForward));
    glm::vec3 pos = this->pos * radius;
    forward = glm::normalize(pos);
    // we normalize here as worldUp and forward
    // are not perpendicular in general
    left = glm::normalize(glm::cross(worldUp, forward));
    up = glm::cross(forward, left);
    view = glm::mat4(1.0f);
    view[0][0] = left.x;
    view[1][0] = left.y;
    view[2][0] = left.z;
    view[3][0] = -glm::dot(left, pos);
    view[0][1] = up.x;
    view[1][1] = up.y;
    view[2][1] = up.z;
    view[3][1] = -glm::dot(up, pos);
    view[0][2] = forward.x;
    view[1][2] = forward.y;
    view[2][2] = forward.z;
    view[3][2] = -glm::dot(forward, pos);
    view *= targetMat;
}
