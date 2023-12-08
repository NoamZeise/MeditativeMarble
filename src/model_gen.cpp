#include "model_gen.h"
#include <graphics/logger.h>
#include <glm/glm.hpp>
#include <functional>

const float PI = 3.141592653589;

struct Var {
    float start;
    float end;
    float step = 1;
};

int quadvi(int i, int j, int j_width, int vert) {
    // 6 inds per quad - v3, v2, v1, v2, v3, v4
    switch(vert) {
    case 1:
	vert = 2;
	break;
    case 2:
	vert = 1;
	break;
    case 3:
	vert = 0;
	break;
    case 4:
	vert = 5;
	break;
    default:
	LOG_ERROR("quadvi vertex out of range!");
	return 0;
    }
    
    const int QUAD_SIZE = 6;
    return (i * j_width  + j) * QUAD_SIZE + vert;
}

ModelInfo::Model genSurface(
	std::function<glm::vec3(float, float)> surfaceFn,
	Var var1, Var var2) {
    ModelInfo::Model model;
    ModelInfo::Mesh m;
    int i = 0;
    int width = (var2.end - var2.start) / var2.step;
    for(float x = var1.start; x < var1.end; x+=var1.step, i++) {
	int j = 0;
	for(float y = var2.start; y < var2.end; y+=var2.step, j++) {
	    int i1, i2, i3, i4;
	    
	    bool doubleEdge = false;

	    if((i == 0 && j == 0) || doubleEdge) {
		m.verticies.push_back(ModelInfo::Vertex());
		i1 = m.verticies.size() - 1;
		m.verticies[i1].Position = surfaceFn(x,y);
	    } else {
		if(i == 0) { 
		    i1 = m.indices[quadvi(i, j-1  , width, 2)];
		} else if(j == 0) {
		    i1 = m.indices[quadvi(i-1, j  , width, 3)];
		} else {
		    i1 = m.indices[quadvi(i-1, j-1, width, 4)];
		}
	    }
	    
	    if(i==0 || doubleEdge) {
		m.verticies.push_back(ModelInfo::Vertex());
		i2 = m.verticies.size() - 1;
		m.verticies[i2].Position = surfaceFn(x,y+var2.step);
	    } else {
		i2 = m.indices[quadvi(i-1,j,width,4)];
	    }
	    
	    if(j==0 || doubleEdge) {
		m.verticies.push_back(ModelInfo::Vertex());
		i3 = m.verticies.size() - 1;
		m.verticies[i3].Position = surfaceFn(x+var1.step,y);	
	    } else {
		i3 = m.indices[quadvi(i, j-1, width, 4)];
	    }

	    m.verticies.push_back(ModelInfo::Vertex());
	    i4 = m.verticies.size() - 1;
	    m.verticies[i4].Position = surfaceFn(x+var1.step,y+var2.step);

	    glm::vec3 v1(m.verticies[i1].Position),
		v2(m.verticies[i2].Position),
		v3(m.verticies[i3].Position),
		v4(m.verticies[i4].Position);
	    
	    // if 2 points of the square are the same, then use the adjacent
	    // edge to calculate the normal
	    glm::vec3 s2 = v2;
	    glm::vec3 s3 = v3;
	    if(v1 == v2)
		s2 = v4;
	    if(v1 == v3)
		s3 = v4;
	    glm::vec3 tri_n1 = glm::cross(
		    v1 - s3,
		    v1 - s2);
	    s2 = v2;
	    s3 = v3;
	    if(v4 == v2)
		s2 = v1;
	    if(v4 == v3)
		s3 = v1;
	    glm::vec3 tri_n2 = glm::cross(		    
		    v4 - s2,
		    v4 - s3);
	    
	    tri_n1 = glm::normalize(tri_n1);
	    tri_n2 = glm::normalize(tri_n2);
	    m.verticies[i1].Normal = tri_n1;
	    m.verticies[i2].Normal = tri_n1 + tri_n2;
	    m.verticies[i2].Normal /= 2;
	    m.verticies[i3].Normal = m.verticies[i2].Normal;
	    m.verticies[i4].Normal = tri_n2;
	    
	    m.indices.push_back(i3);
	    m.indices.push_back(i2);
	    m.indices.push_back(i1);
	    m.indices.push_back(i2);
	    m.indices.push_back(i3);
	    m.indices.push_back(i4);
	}
    }
    m.diffuseColour = glm::vec4(0.4, 0.8, 0.1, 1);
    model.meshes = {m};
    return model;
}

glm::vec3 sphere(float theta, float phi) {
    float rad = 5;
    return {
	rad*sin(theta)*cos(phi),
	rad*sin(theta)*sin(phi),
    	rad*cos(theta)};
}

ModelInfo::Model genSphere() {
    return genSurface(sphere, {0, PI, 0.04f*PI}, {0, 2*PI, 0.02f*PI});
}

glm::vec3 land(float x, float y) {
    return glm::vec3(x, y, sin(0.1*x)*cos(0.3*y)*2 - 4);
}

ModelInfo::Model genModel() {
    return genSurface(land, {-100, 100, 1}, {-100, 100, 1});
}

