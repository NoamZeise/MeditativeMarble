#include <manager.h>
#include <GameHelper/camera.h>
#include <graphics/glm_helper.h>
#include <graphics/logger.h>
#include <glm/gtc/matrix_inverse.hpp>

#include "third_person_cam.h"
#include "debug.h"

int main() {
    ManagerState state;
    state.cursor = cursorState::disabled;
    state.windowTitle = "bmjam";
    state.windowWidth = 800;
    state.windowHeight = 600;
    state.conf.multisampling = true;
    state.conf.sample_shading = true;

    Manager manager(state);
    ResourcePool* pool = manager.render->pool();
    Resource::Model sphere = pool->model()->load("models/sphere.obj");
    glm::mat4 sphereMat = glm::mat4(1.0f);
    Resource::Texture testImage = pool->tex()->load("textures/test.png");
    Resource::Model monkey = pool->model()->load("models/monkey.obj");
    glm::mat4 monkeyMat = glm::translate(glm::mat4(1.0f), glm::vec3(2, 0, 0));
    debug::setFont(pool->font()->load("textures/Roboto-Black.ttf"));
    
    manager.render->LoadResourcesToGPU(pool);
    manager.render->UseLoadedResources();

    glm::vec3 spherePos(0);

    ThirdPersonCam cam;
    float camRad = 5.0f;

    while(!glfwWindowShouldClose(manager.window)) {
	// Update
	manager.update();
	if(manager.input.kb.press(GLFW_KEY_ESCAPE))
	    glfwSetWindowShouldClose(manager.window, GLFW_TRUE);

	cam.control(manager.input, manager.timer.dt());

	float speed = 0.01 * manager.timer.dt();
	if(manager.input.kb.hold(GLFW_KEY_W))
	    spherePos += speed * cam.getTargetForward();
	if(manager.input.kb.hold(GLFW_KEY_S))
	    spherePos -= speed * cam.getTargetForward();
	if(manager.input.kb.hold(GLFW_KEY_A))
	    spherePos += speed * cam.getTargetLeft();
	if(manager.input.kb.hold(GLFW_KEY_D))
	    spherePos -= speed * cam.getTargetLeft();
	camRad += -4*speed * manager.input.m.scroll();
	sphereMat = glm::translate(glm::mat4(1.0f), glm::vec3(spherePos.x, spherePos.y, spherePos.z));

	cam.setTarget(spherePos, camRad);
	manager.render->set3DViewMat(cam.getView(), cam.getPos());
	
	if(manager.winActive()) {
	    manager.render->DrawModel(sphere, sphereMat, glm::inverseTranspose(sphereMat));
	    manager.render->DrawModel(monkey, monkeyMat, glm::inverseTranspose(monkeyMat));
	    std::atomic<bool> drawSubmitted;
	    manager.render->EndDraw(drawSubmitted);
	}
    }
}
