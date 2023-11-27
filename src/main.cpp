#include <manager.h>
#include <GameHelper/camera.h>
#include <graphics/glm_helper.h>
#include <glm/gtc/matrix_inverse.hpp> //for inverseTranspose

int main() {
    ManagerState state;
    state.cursor = cursorState::disabled;
    state.windowTitle = "bmjam";
    state.windowWidth = 800;
    state.windowHeight = 600;

    Manager manager(state);
    ResourcePool* pool = manager.render->pool();
    Resource::Model monkey = pool->model()->load("models/monkey.obj");
    glm::mat4 monkeyMat = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(270.0f),
						     glm::vec3(-1.0f, 0.0f, 0.0f)),
					 glm::vec3(0.0f, -8.0f, -10.0f));    
    Resource::Texture testImage = pool->tex()->load("textures/test.png");
    manager.render->LoadResourcesToGPU(pool);
    manager.render->UseLoadedResources();
    camera::FirstPerson cam;
    while(!glfwWindowShouldClose(manager.window)) {
	manager.update();
	if(manager.input.kb.press(GLFW_KEY_ESCAPE))
	    glfwSetWindowShouldClose(manager.window, GLFW_TRUE);
	cam.update(manager.input, manager.timer);
	manager.fov = cam.getZoom();
	manager.render->set3DViewMat(cam.getViewMatrix(), cam.getPos());

	if(manager.winActive()) {
	    manager.render->DrawQuad(testImage,
				     glmhelper::calcMatFromRect(
					     glm::vec4(0, 0, 400, 600),
					     0.0f, 0.001f));
	    manager.render->DrawQuad(testImage,
				     glmhelper::calcMatFromRect(
					     glm::vec4(400, 0, 400, 600),
					     0.0f, 4.9f));
	    manager.render->DrawModel(monkey, monkeyMat, glm::inverseTranspose(monkeyMat));
	    std::atomic<bool> drawSubmitted;
	    manager.render->EndDraw(drawSubmitted);
	}
    }
}
