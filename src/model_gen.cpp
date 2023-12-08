#include "model_gen.h"
#include <glm/glm.hpp>
#include <functional>

const float PI = 3.141592653589;

glm::vec3 sphere(float theta, float phi) {
    float rad = 5;
    return {
	rad*sin(theta)*cos(phi),
	rad*sin(theta)*sin(phi),
    	rad*cos(theta)};
}

struct Var {
    float start;
    float end;
    float step = 1;
};

ModelInfo::Model genSurface(
	std::function<glm::vec3(float, float)> surfaceFn,
	Var var1, Var var2) {
   ModelInfo::Model model;
    ModelInfo::Mesh m;
    
    for(float x = var1.start; x < var1.end; x+=var1.step) {
	for(float y = var2.start; y < var2.end; y+=var2.step) {
	    ModelInfo::Vertex v1, v2, v3, v4;

	    v1.Position = surfaceFn(x,        y);
	    v2.Position = surfaceFn(x,        y+var2.step);
	    v3.Position = surfaceFn(x+var1.step, y);
	    v4.Position = surfaceFn(x+var1.step, y+var2.step);

	    // if 2 points of the square are the same, then use the adjacent
	    // edge to calculate the normal
	    glm::vec3 s2 = v2.Position;
	    glm::vec3 s3 = v3.Position;
	    if(v1.Position == v2.Position)
		s2 = v4.Position;
	    if(v1.Position == v3.Position)
		s3 = v4.Position;
	    glm::vec3 tri_n1 = glm::cross(
		    v1.Position - s3,
		    v1.Position - s2);
	    s2 = v2.Position;
	    s3 = v3.Position;
	    if(v4.Position == v2.Position)
		s3 = v1.Position;
	    if(v4.Position == v3.Position)
		s2 = v1.Position;
	    glm::vec3 tri_n2 = glm::cross(		    
		    v4.Position - s2,
		    v4.Position - s3);
	    
	    tri_n1 = glm::normalize(tri_n1);
	    tri_n2 = glm::normalize(tri_n2);
	    v1.Normal = tri_n1;
	    v2.Normal = tri_n1 + tri_n2;
	    v2.Normal /= 2;
	    v3.Normal = v2.Normal;
	    v4.Normal = tri_n2;
	    
	    int vi = m.verticies.size();
	    m.verticies.push_back(v1);
	    m.verticies.push_back(v2);
	    
	    m.verticies.push_back(v3);
	    m.verticies.push_back(v4);
	    m.indices.push_back(vi + 2);
	    m.indices.push_back(vi + 1);
	    m.indices.push_back(vi);
	    m.indices.push_back(vi + 1);
	    m.indices.push_back(vi + 2);
	    m.indices.push_back(vi + 3);
	}
    }
    m.diffuseColour = glm::vec4(0.4, 0.8, 0.1, 1);
    model.meshes = {m};
    return model;
}


ModelInfo::Model genModel() {
    return genSurface(sphere, {0, PI, 0.04f*PI}, {0, 2*PI, 0.02f*PI});
}

