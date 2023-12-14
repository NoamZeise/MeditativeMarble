#include "player.h"

#include <glm/gtc/matrix_inverse.hpp>

Obj3D::Obj3D(Resource::Model model) {
    this->model = model;
    updateMat();
}

void Obj3D::setPos(glm::vec3 pos) {
    this->modelPos = pos;
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
		modelPos);
    normalMat = glm::inverseTranspose(modelMat);
}

void Obj3D::Draw(Render *render) {
    render->DrawModel(model, modelMat, normalMat, colour);
}

/// --- Player ---

Player::Player(Resource::Model model) : Obj3D(model) {
    radius = 1;
    setScale(glm::vec3(radius));
}

void Player::Update(Input &input, Timer &timer,
		    glm::vec3 forward, glm::vec3 left) {
    Obj3D::setPos(PhysObj::getPos());
    
    glm::vec3 acceleration(0);
    if(input.kb.hold(GLFW_KEY_W) || input.kb.hold(GLFW_KEY_UP))
	acceleration += speed * forward;
    if(input.kb.hold(GLFW_KEY_S) || input.kb.hold(GLFW_KEY_DOWN))
	acceleration -= speed * forward;
    if(input.kb.hold(GLFW_KEY_A) || input.kb.hold(GLFW_KEY_LEFT))
	acceleration += speed * left;
    if(input.kb.hold(GLFW_KEY_D) || input.kb.hold(GLFW_KEY_RIGHT))
	acceleration -= speed * left;
    if(input.kb.hold(GLFW_KEY_SPACE) && isGrounded())
	PhysObj::addVelocity(collisionN * 0.1f);
    if(input.kb.hold(GLFW_KEY_LEFT_SHIFT) && !isGrounded())
	acceleration.z -= 0.0005f;
    setAcceleration(acceleration);
}

void Player::Draw(Render* render) {
    Obj3D::Draw(render);
    glm::mat4 debug = modelMat *
	glm::scale(glm::mat4(1.0f),
		   glm::vec3(0.5));
    render->DrawModel(model, glm::translate(
			      debug, 2.0f*collisionN),
		      normalMat);
    render->DrawModel(model, glm::translate(
			      debug, 2.0f*collisionT),
		      normalMat);
    render->DrawModel(model, glm::translate(
			      debug, 4.0f*glm::normalize(spinAxis)),
		      normalMat);
}
