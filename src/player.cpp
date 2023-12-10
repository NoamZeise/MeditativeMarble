#include "player.h"

#include <glm/gtc/matrix_inverse.hpp>

Obj3D::Obj3D(Resource::Model model) {
    this->model = model;
    updateMat();
}

void Obj3D::setPos(glm::vec3 pos) {
    this->pos = pos;
    updateMat();
}

void Obj3D::setScale(glm::vec3 scale) {
    this->scale = scale;
    updateMat();
}

void Obj3D::setColour(glm::vec4 colour) {
    this->colour = colour;
}

void Obj3D::updateMat() {
    modelMat =
	glm::translate(
		glm::scale(
			glm::mat4(1.0f),
			scale),
		pos);
    normalMat = glm::inverseTranspose(modelMat);
}

void Obj3D::Draw(Render *render) {
    render->DrawModel(model, modelMat, normalMat, colour);
}



/// --- Player ---

Player::Player(Resource::Model model) : Obj3D(model) {}

void Player::Update(Input &input, Timer &timer,
		    glm::vec3 forward, glm::vec3 left) {
    float speed = movespeed * timer.dt();
    glm::vec3 spherePos = getPos();
    if(input.kb.hold(GLFW_KEY_W))
	spherePos += speed * forward;
    if(input.kb.hold(GLFW_KEY_S))
	spherePos -= speed * forward;
    if(input.kb.hold(GLFW_KEY_A))
	spherePos += speed * left;
    if(input.kb.hold(GLFW_KEY_D))
	spherePos -= speed * left;
    if(input.kb.hold(GLFW_KEY_SPACE))
	spherePos.z += speed;
    if(input.kb.hold(GLFW_KEY_LEFT_SHIFT))
	spherePos.z -= speed;
    if(spherePos != getPos())
	setPos(spherePos);
}
