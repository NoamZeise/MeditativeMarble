#include "model_gen.h"
#include <glm/glm.hpp>

ModelInfo::Model genModel() {
    ModelInfo::Model model;
    ModelInfo::Mesh m;
    
    float height = -5;
    float sf = 0.2;
    float cf = 0.3;
    int first_i = -100;
    int first_j = -100;
    for(int i = first_i; i < 100; i++) {
	for(int j = first_j; j < 100; j++) {
	    ModelInfo::Vertex v1, v2, v3, v4;
	    
	    v1.Position = glm::vec3(0 + i, 0 + j , height + sin(i*sf) + cos(j*cf));
	    v2.Position = glm::vec3(0 + i, 1 + j, height+ sin(i*sf) + cos((1+j)*cf));
	    v3.Position = glm::vec3(1 + i, 0 + j, height+ sin((1+i)*sf) + cos(j*cf));
	    v4.Position = glm::vec3(1 + i, 1 + j, height+ sin((1+i)*sf) + cos((1+j)*cf));

	    glm::vec3 tri_n1 = glm::cross(
		    v1.Position - v3.Position,
		    v1.Position - v2.Position);
	    glm::vec3 tri_n2 = glm::cross(		    
		    v4.Position - v2.Position,
		    v4.Position - v3.Position);
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
    m.diffuseColour = glm::vec4(0.2, 0.6, 0.05, 1);
    model.meshes = {m};
    return model;
}
