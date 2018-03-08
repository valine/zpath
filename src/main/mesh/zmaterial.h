#ifndef ZMATERIAL_H
#define ZMATERIAL_H
#include <vector>
#include "mesh/zmesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

class ZMaterial {

public:
	ZMaterial(vec3 color);
	void setColor(vec3 color);
	vec3 getColor();

private:
	vec3 mColor;
};

#endif