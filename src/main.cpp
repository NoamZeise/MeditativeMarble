#include <manager.h>
#include <graphics/glm_helper.h>
#include <graphics/logger.h>
#include <graphics/model/info.h>
#include <glm/gtc/matrix_inverse.hpp>

#include "third_person_cam.h"
#include "player.h"
#include "world.h"
#include "debug.h"

int main(int argc, char** argv) {
    ManagerState state;
    state.cursor = cursorState::disabled;
    state.windowTitle = "bmjam";
    state.windowWidth = 800;
    state.windowHeight = 600;
    state.conf.multisampling = true;
    state.conf.sample_shading = true;
    if(argc > 1) {
	if(std::string(argv[1]) == "opengl")
	    state.defaultRenderer = RenderFramework::OpenGL;
    }

    Manager manager(state);
    ResourcePool* pool = manager.render->pool();
    Player player(pool->model()->load("models/sphere.obj"));
    Resource::Texture testImage = pool->tex()->load("textures/test.png");
    debug::setFont(pool->font()->load("textures/Roboto-Black.ttf"));
    World world(pool->model());
    
    manager.render->LoadResourcesToGPU(pool);
    manager.render->UseLoadedResources();

    ThirdPersonCam cam;
    float camRad = 5.0f;
    
    std::string drawStats, updateStats;
    while(!glfwWindowShouldClose(manager.window)) {
	// Update
	auto start = std::chrono::high_resolution_clock::now();
	manager.update();
	if(manager.input.kb.press(GLFW_KEY_ESCAPE))
	    glfwSetWindowShouldClose(manager.window, GLFW_TRUE);

	player.Update(manager.input, manager.timer, cam.getTargetForward(), cam.getTargetLeft());
	player.PhysObj::Update(manager.timer.dt());
	if(world.checkCollision(player.PhysObj::getPos())) {
	    player.fixPos(player.PhysObj::getPos() - glm::vec3(0, 0, 1));	    
	}
	cam.control(manager.input, manager.timer.dt());
	camRad += -0.04*manager.timer.dt() * manager.input.m.scroll();
	cam.setTarget(player.Obj3D::getPos(), camRad);
	manager.render->set3DViewMat(cam.getView(), cam.getPos());

	updateStats = std::to_string(
		std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now() - start).count() / 1000.0)
	    + " ms";
	
	if(manager.winActive()) {
	    player.Draw(manager.render);
	    world.Draw(manager.render);
	    debug::draw(manager.render, 30, "update", updateStats);
	    debug::draw(manager.render, 50, "draw", drawStats);
	    std::atomic<bool> drawSubmitted;
	    manager.render->EndDraw(drawSubmitted);
	    drawStats = std::to_string(
		    1.0 / std::chrono::duration_cast<std::chrono::microseconds>(
			    std::chrono::high_resolution_clock::now() - start).count() * 1000000.0) + " fps";
	}
    }
}
