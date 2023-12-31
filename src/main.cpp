#include <manager.h>
#include <graphics/glm_helper.h>
#include <graphics/logger.h>
#include <graphics/model/info.h>
#include <glm/gtc/matrix_inverse.hpp>

#include "third_person_cam.h"
#include "player.h"
#include "world.h"

int main(int argc, char** argv) {
    ManagerState state;
    state.cursor = cursorState::disabled;
    state.windowTitle = "Meditative Marble";
    state.windowWidth = 800;
    state.windowHeight = 600;
    state.conf.multisampling = true;
    state.conf.sample_shading = true;
    state.conf.depth_range_3D[1] = 10000.0f;
    state.conf.clear_colour[0] = 0/255.0f;
    state.conf.clear_colour[1] = 198/255.0f;
    state.conf.clear_colour[2] = 224/255.0f;
    
    if(argc > 1) {
	if(std::string(argv[1]) == "opengl")
	    state.defaultRenderer = RenderFramework::OpenGL;
    }

    Manager manager(state);
    ResourcePool* pool = manager.render->pool();

    ModelInfo::Model sphere = pool->model()->loadModelData("models/sphere.obj");
    sphere.meshes[0].diffuseTextures.push_back("player.png");
    Player player(pool->model()->load(sphere));
    World world(manager.render, manager.backend() != RenderFramework::OpenGL);
    PhysicsManager pm(&world);
    pm.addPhysObj(&player);
    
    manager.render->LoadResourcesToGPU(pool);
    manager.render->UseLoadedResources();

    BPLighting light;
    light.direction = glm::vec4(0.3, 0.5, -1, 0);
    manager.render->setLightingProps(light);

    ThirdPersonCam cam;
    float camRad = INITAL_CAM_RAD;
    while(!glfwWindowShouldClose(manager.window)) {
	manager.update();
	if(manager.input.kb.press(GLFW_KEY_ESCAPE))
	    glfwSetWindowShouldClose(manager.window, GLFW_TRUE);
	if(manager.input.kb.press(GLFW_KEY_F))
	    manager.toggleFullscreen();

	player.Update(manager.input, manager.timer, cam.getTargetForward(), cam.getTargetLeft());
	world.Update(player.PhysObj::getPos(), player.getVel());
	if(world.recreationRequired())
	    manager.render->UseLoadedResources();
	pm.Update(manager.timer.dt());

	cam.setPos(pm.fixCamPos(cam.getLocalPos(), &camRad, player.PhysObj::getPos(),
				      player.PhysObj::getVel()));
	cam.setTarget(player.Obj3D::getPos(), camRad);
	
	manager.render->set3DViewMat(cam.getView(), cam.getPos());
	if(manager.winActive()) {
	    player.Draw(manager.render);
	    world.Draw(manager.render);
	    std::atomic<bool> drawSubmitted;
	    manager.render->EndDraw(drawSubmitted);
	}
    }
}
