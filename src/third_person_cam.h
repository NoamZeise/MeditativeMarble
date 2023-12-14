#ifndef THIRD_PERSON_CAM_H
#define THIRD_PERSON_CAM_H

#include <glm/glm.hpp>
#include <game/input.h>

class ThirdPersonCam {
 public:
    ThirdPersonCam();
    void control(Input &input, double dt);
    void setTarget(glm::vec3 target, float radius);
    glm::mat4 getView() { return view; }
    glm::vec3 getPos() { return worldPos; }
    void setTempRadius(float r);
    void setPos(glm::vec3 pos);
    glm::vec3 getLocalPos() { return pos; }
    void setWorldUp(glm::vec3 worldUp);
    glm::vec3 getWorldUp() { return worldUp; }
    glm::vec3 getTargetForward() { return targetForward; }
    glm::vec3 getTargetLeft() { return targetLeft; }
 private:
    void updateView();
    
    // usually unchanging camera settings
    float mouseSensitivity = 0.1f;
    float lim = 0.9;
    glm::vec3 worldUp = glm::normalize(glm::vec3(0, 0, 1));

    //local space props
    glm::vec3 pos;
    glm::vec3 forward, up, left;

    // target/world space stuff
    float radius = 1.0f;
    glm::vec3 target;
    glm::mat4 targetMat = glm::mat4(1.0f);
    glm::vec3 worldPos;
    glm::mat4 view;
    glm::vec3 targetForward;
    glm::vec3 targetLeft;
};


#endif /* THIRD_PERSON_CAM_H */

