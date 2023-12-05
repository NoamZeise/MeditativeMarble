#include <manager.h>
#include <GameHelper/camera.h>
#include <graphics/glm_helper.h>
#include <graphics/logger.h>
#include <glm/gtc/matrix_inverse.hpp>

void draw_vect(Render* render,
	       Resource::Font font,
	       std::string vname,
	       glm::vec3 v,
	       float y);

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
    Resource::Font font = pool->font()->load("textures/Roboto-Black.ttf");
    
    manager.render->LoadResourcesToGPU(pool);
    manager.render->UseLoadedResources();

    glm::vec3 camPos(5, 5, 0);
    glm::vec3 target(0);
    glm::vec3 up(0, 0, 1);

    glm::vec3 camForward = glm::normalize(camPos - target);
    glm::vec3 camLeft = glm::normalize(glm::cross(up, camForward));
    glm::vec3 camUp = glm::normalize(glm::cross(camForward, camLeft));
    
    while(!glfwWindowShouldClose(manager.window)) {
	// Update
	manager.update();
	if(manager.input.kb.press(GLFW_KEY_ESCAPE))
	    glfwSetWindowShouldClose(manager.window, GLFW_TRUE);

	camForward = glm::normalize(camPos - target);
	camLeft = glm::normalize(glm::cross(up, camForward));
	camUp = glm::normalize(glm::cross(camForward, camLeft));
	glm::mat4 camView(1.0f);
	camView[0][0] = camLeft.x;
	camView[1][0] = camLeft.y;
	camView[2][0] = camLeft.z;
	camView[3][0] = -glm::dot(camLeft, camPos);
	camView[0][1] = camUp.x;
	camView[1][1] = camUp.y;
	camView[2][1] = camUp.z;
	camView[3][1] = -glm::dot(camUp, camPos);
	camView[0][2] = camForward.x;
	camView[1][2] = camForward.y;
	camView[2][2] = camForward.z;
	camView[3][2] = -glm::dot(camForward, camPos);

	float speed = 0.01 * manager.timer.dt();
	if(manager.input.kb.hold(GLFW_KEY_UP))
	    target.y -= speed;
	if(manager.input.kb.hold(GLFW_KEY_DOWN))
	    target.y += speed;
	if(manager.input.kb.hold(GLFW_KEY_LEFT))
	    target.x -= speed;
	if(manager.input.kb.hold(GLFW_KEY_RIGHT))
	    target.x += speed;
	sphereMat = glm::translate(glm::mat4(1.0f), target);

	float mouseSensitivity = 0.1f;
	glm::vec2 ctrlDir(-manager.input.m.dx(), -manager.input.m.dy());
	ctrlDir *= mouseSensitivity;
	if(ctrlDir.x != 0 || ctrlDir.y != 0) {
	    float updot = glm::dot(camForward, up);
	    float lim = 0.9f;
	    if(updot > lim && ctrlDir.y < 0 || -updot > lim && ctrlDir.y > 0)
		ctrlDir.y = 0;
	    float size = 0.001 * manager.timer.dt();
	    ctrlDir *= size/2;
	    auto qx = glm::quat(cos(ctrlDir.x), (float)sin(ctrlDir.x)*up);
	    auto qy = glm::quat(cos(ctrlDir.y), (float)sin(ctrlDir.y)*camLeft);
	    auto q = qx * qy;
	    auto c = glm::conjugate(q);
	    camPos = q * camPos * c;
	}
	
	manager.render->set3DViewMat(camView, camPos);
	
	// Draw
	if(manager.winActive()) {
	    manager.render->DrawModel(sphere, sphereMat, glm::inverseTranspose(sphereMat));
	    manager.render->DrawModel(monkey, monkeyMat, glm::inverseTranspose(monkeyMat));
	    draw_vect(manager.render, font, "cam pos", camPos, 50.0);
	    std::atomic<bool> drawSubmitted;
	    manager.render->EndDraw(drawSubmitted);
	}
    }
}

void draw_vect(Render* render,
	       Resource::Font font,
	       std::string vname,
	       glm::vec3 v, float y) {
    render->DrawString(font, vname + ": ("
		       + std::to_string(v.x) + ", "
		       + std::to_string(v.y) + ", "
		       + std::to_string(v.z) + ")",
		       glm::vec2(10, y), 20, 0.9f, glm::vec4(1.0f)); 
}
