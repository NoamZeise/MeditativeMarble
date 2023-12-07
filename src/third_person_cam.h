#ifndef THIRD_PERSON_CAM_H
#define THIRD_PERSON_CAM_H

#include <glm/glm.hpp>
#include <GameHelper/input.h>

class ThirdPersonCam {
 public:
    ThirdPersonCam();
    void control(gamehelper::Input &input, double dt);
    void setTarget(glm::vec3 target, float radius);
    glm::mat4 getView() { return view; }
    glm::vec3 getPos() { return worldPos; }
 private:

    void updateView();
    
    // unchanging camera settings
    float mouseSensitivity = 0.1f;
    float lim = 0.9;
    glm::vec3 worldUp = glm::vec3(0, 0, 1);

    //local space props
    glm::vec3 pos;
    glm::vec3 forward, up, left;

    // target space
    float radius = 1.0f;
    glm::vec3 target = glm::vec3(0, 0, 0);
    glm::mat4 targetMat = glm::mat4(1.0f);
    glm::vec3 worldPos;
    glm::mat4 view;
};


#endif /* THIRD_PERSON_CAM_H */

