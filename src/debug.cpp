#include "debug.h"
#include <graphics/logger.h>

Resource::Font f;
bool fontSet = false;

namespace debug {

  void setFont(Resource::Font font) {
      f = font;
      fontSet = true;
  }

  void draw(Render *render, float height, std::string varName, std::string text) {
      if (!fontSet) {
	  LOG_ERROR("app:debug.cpp error, no font set!");
	  return;
      }
      render->DrawString(f,
			 varName + " : " + text,
			 glm::vec2(10, height), 20, 0.9f, glm::vec4(1.0f));
  }

  void draw(Render *render, float height, std::string varName, glm::vec3 vec) {
      draw(render, height, varName,
	   "(" + std::to_string(vec.x) + ", "
	       + std::to_string(vec.y) + ", "
	       + std::to_string(vec.z) + ")");
  }

  void draw(Render *render, float height, std::string varName, long long f) {
      draw(render, height, varName, std::to_string(f));
  }

}
