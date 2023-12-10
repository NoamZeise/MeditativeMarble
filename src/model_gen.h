#ifndef MODEL_GEN_H
#define MODEL_GEN_H

#include <graphics/model/info.h>
#include <functional>

struct SurfaceParam {
    float start;
    float end;
    float step = 1;
};

ModelInfo::Model genSurface(
	std::function<glm::vec3(float, float)> surfaceFn,
	bool smoothShading,
	SurfaceParam a,
	SurfaceParam b);

#endif /* MODEL_GEN_H */

