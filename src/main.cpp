#include <manager.h>
#include <graphics/glm_helper.h>
#include <graphics/logger.h>
#include <graphics/model/info.h>
#include <glm/gtc/matrix_inverse.hpp>

#include "third_person_cam.h"
#include "model_gen.h"
#include "player.h"
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

    ModelInfo::Model gennedModelInfo = genModel();
    Resource::Model genM = pool->model()->load(gennedModelInfo);
    glm::mat4 genMat(1.0f);

    
    manager.render->LoadResourcesToGPU(pool);
    manager.render->UseLoadedResources();

    ThirdPersonCam cam;
    float camRad = 5.0f;

    while(!glfwWindowShouldClose(manager.window)) {
	// Update
	manager.update();
	if(manager.input.kb.press(GLFW_KEY_ESCAPE))
	    glfwSetWindowShouldClose(manager.window, GLFW_TRUE);

	player.Update(manager.input, manager.timer, cam.getTargetForward(), cam.getTargetLeft());
	
	cam.control(manager.input, manager.timer.dt());
	camRad += -0.04*manager.timer.dt() * manager.input.m.scroll();
	cam.setTarget(player.getPos(), camRad);
	manager.render->set3DViewMat(cam.getView(), cam.getPos());
	
	if(manager.winActive()) {
	    player.Draw(manager.render);
	    manager.render->DrawModel(genM, genMat, glm::inverseTranspose(genMat));
	    std::atomic<bool> drawSubmitted;
	    manager.render->EndDraw(drawSubmitted);
	}
    }
}
