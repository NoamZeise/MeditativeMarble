#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include <graphics/render.h>
#include <string>

#define DEBUG_DRAW(render, height, variable)                                   \
  debug::draw(render, height, #variable, variable);


namespace debug {
    void setFont(Resource::Font font);
    void draw(Render *render, float height, std::string varName, std::string text);
    void draw(Render *render, float height, std::string varName, glm::vec3 vec);
}

#endif /* DEBUG_DRAW_H */
