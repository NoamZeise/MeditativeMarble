#ifndef PLAYER_H
#define PLAYER_H

#include <graphics/render.h>
#include <game/input.h>
#include <game/timer.h>
#include "physics.h"

class Obj3D {
 public:
    Obj3D() {}
    Obj3D(Resource::Model model);
    virtual void Draw(Render *render);
    void setPos(glm::vec3 pos);
    glm::vec3 getPos() { return modelPos; }
    void setScale(glm::vec3 scale);
    void setRot(glm::vec3 axis, float angle);
    glm::vec3 getScale() { return scale; }
    void setColour(glm::vec4 colour);
private:
    void updateMat();
    glm::vec3 modelPos = glm::vec3(0, 0, 0);
    glm::vec3 scale = glm::vec3(1, 1, 1);
    glm::vec4 colour = glm::vec4(1.0f);
    glm::vec3 axis = glm::vec3(0, 0, 1);
    float angle = 0;
    
    glm::mat4 modelMat = glm::mat4(1.0f);
    Resource::Model model;
    glm::mat4 normalMat;
};

class Player : public Obj3D, public Sphere {
 public:
    Player() {}
    Player(Resource::Model model);
    void Update(Input &input, Timer &timer,
		glm::vec3 forward, glm::vec3 left);
 private:
    float speed = 0.00003;
    float spinTime = 0;
};

#endif /* PLAYER_H */
