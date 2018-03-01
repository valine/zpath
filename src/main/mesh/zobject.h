#ifndef ZOBJECT_H
#define ZOBJECT_H
#include <vector>
#include "mesh/zmesh.h"
using std::vector;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;
class ZObject {
	
public:
	ZObject();
	void setMesh(ZMesh* mesh);
	ZMesh* getMesh();
	void setColor(vec3 color);
	vec3 getColor();

private: 
	ZMesh* mMesh;
	vec3 mColor;
};

#endif