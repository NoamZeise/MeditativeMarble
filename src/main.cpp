#include <manager.h>
#include <GameHelper/camera.h>
#include <graphics/glm_helper.h>
#include <glm/gtc/matrix_inverse.hpp> //for inverseTranspose

int main() {
    ManagerState state;
    state.cursor = cursorState::normal;
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
    
    manager.render->LoadResourcesToGPU(pool);
    manager.render->UseLoadedResources();

    glm::vec3 camPos(5, 0, 0);
    glm::vec3 target(0);
    glm::vec3 up(0, 0, 1);
    
    while(!glfwWindowShouldClose(manager.window)) {
	// Update
	manager.update();
	if(manager.input.kb.press(GLFW_KEY_ESCAPE))
	    glfwSetWindowShouldClose(manager.window, GLFW_TRUE);

	if(manager.input.kb.hold(GLFW_KEY_UP)) {
	    auto r = glm::quat(glm::vec3(0, 0.001 * manager.timer.dt(), 0));
	    auto c = glm::conjugate(r);
	    camPos = r * camPos * c;
	}
	if(manager.input.kb.hold(GLFW_KEY_DOWN)) {
	    auto r = glm::quat(glm::vec3(0, -0.001 * manager.timer.dt(), 0));
	    auto c = glm::conjugate(r);
	    camPos = r * camPos * c;
	}
	
	manager.render->set3DViewMat(glm::lookAt(camPos, target, up),
				     camPos);
	// Draw
	if(manager.winActive()) {
	    /*manager.render->DrawQuad(testImage,
				     glmhelper::calcMatFromRect(
					     glm::vec4(0, 0, 800, 600),
					     0.0f, 0.001f));*/
	    manager.render->DrawModel(sphere, sphereMat, glm::inverseTranspose(sphereMat));
	    manager.render->DrawModel(monkey, monkeyMat, glm::inverseTranspose(monkeyMat));
	    std::atomic<bool> drawSubmitted;
	    manager.render->EndDraw(drawSubmitted);
	}
    }
}
