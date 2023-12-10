#ifndef PLAYER_H
#define PLAYER_H

#include <graphics/render.h>
#include <game/input.h>
#include <game/timer.h>

class Obj3D {
 public:
    Obj3D() {}
    Obj3D(Resource::Model model);
    void Draw(Render *render);
    void setPos(glm::vec3 pos);
    glm::vec3 getPos() { return pos; }
    void setScale(glm::vec3 scale);
    glm::vec3 getScale() { return scale; }
    void setColour(glm::vec4 colour);
 private:
    void updateMat();
    glm::vec3 pos = glm::vec3(0, 0, 0);
    glm::vec3 scale = glm::vec3(1, 1, 1);
    glm::vec4 colour = glm::vec4(1.0f);

    glm::mat4 modelMat = glm::mat4(1.0f);
    Resource::Model model;
    glm::mat4 normalMat;
};

class Player : public Obj3D {
 public:
    Player() {}
    Player(Resource::Model model);
    void Update(Input &input, Timer &timer,
		glm::vec3 forward, glm::vec3 left);
 private:
    float movespeed = 0.01;
};

#endif /* PLAYER_H */
