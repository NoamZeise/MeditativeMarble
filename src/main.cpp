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
    state.conf.depth_range_3D[1] = 10000.0f;
    if(argc > 1) {
	if(std::string(argv[1]) == "opengl")
	    state.defaultRenderer = RenderFramework::OpenGL;
    }

    Manager manager(state);
    ResourcePool* pool = manager.render->pool();
    
    Resource::Texture testImage = pool->tex()->load("textures/test.png");
    debug::setFont(pool->font()->load("textures/Roboto-Black.ttf"));

    Player player(pool->model()->load("models/sphere.obj"));
    World world(manager.render);
    PhysicsManager pm(&world);
    pm.addPhysObj(&player);
    
    manager.render->LoadResourcesToGPU(pool);
    LOG("to load res");
    manager.render->UseLoadedResources();

    BPLighting light;
    light.direction = glm::vec4(0.3, 0.5, -1, 0);
    manager.render->setLightingProps(light);

    ThirdPersonCam cam;
    const float INITAL_CAM_RAD = 20.0f;
    float camRad = INITAL_CAM_RAD;
    LOG("to draw");
    std::string drawStats, updateStats;
    while(!glfwWindowShouldClose(manager.window)) {
	// Update
	auto start = std::chrono::high_resolution_clock::now();
	manager.update();
	if(manager.input.kb.press(GLFW_KEY_ESCAPE))
	    glfwSetWindowShouldClose(manager.window, GLFW_TRUE);

	player.Update(manager.input, manager.timer, cam.getTargetForward(), cam.getTargetLeft());
	world.Update(player.PhysObj::getPos());
	pm.Update(manager.timer.dt());

	cam.control(manager.input, manager.timer.dt());
	camRad += -0.04*manager.timer.dt() * manager.input.m.scroll();
	
	glm::vec3 pv = player.getVel();
	pv.z = 0;
	float mod = glm::length(pv)*100.0f;
	if(mod < INITAL_CAM_RAD)
	    mod = INITAL_CAM_RAD;
	float change = camRad - mod;
	camRad = (camRad + mod)*0.5f;
	//	manager.fov = 45.0f + (change*0.1f)*10.0f;
	pv.z = -0.01;
	glm::vec3 cp = -pv + cam.getLocalPos();
	if(cp != glm::vec3(0)) {
	    cp = glm::normalize(cp);
	    glm::vec3 camPos = cp * camRad + player.PhysObj::getPos();
	    glm::vec3 np = world.nearestPoint(camPos);
	    glm::vec3 n = camPos - np;
	    float d = n.z;
	    float dp = (player.PhysObj::getPos().z - camPos.z);
	    //LOG("d: " << d << " - p: " << dp << " - mod: " << mod);
	    float fromSurface = 4.0f;
	    if(d < fromSurface && n != glm::vec3(0)) {
		glm::normalize(n);
		if(d < 0) {
		    n *= -1.0f;
		    d = 0;
		}
		glm::vec3 newWorldPos = camPos + (float)(fabs(d - fromSurface))*n;
		float t = (fromSurface - d)/fromSurface;
		glm::vec3 mid = camPos +
		    (t)*(newWorldPos - camPos);
		cp = (1/camRad)*(mid - player.PhysObj::getPos());
		if(cp != glm::vec3(0))
		    cp = glm::normalize(cp);
		else
		    cp = glm::vec3(0, 0, 1);
	    }	    
	    cam.setPos(cp);
	}
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
	    debug::draw(manager.render, 70, "velocity", player.getVel());
	    std::atomic<bool> drawSubmitted;
	    manager.render->EndDraw(drawSubmitted);
	    drawStats = std::to_string(
		    1.0 / std::chrono::duration_cast<std::chrono::microseconds>(
			    std::chrono::high_resolution_clock::now() - start).count() * 1000000.0) + " fps";
	}
    }
}
