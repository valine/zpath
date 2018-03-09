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

	vec3 getColor();
	void setColor(vec3 color);

	float getMetallic();
	void setMetallic(float factor);

	float getRoughness();
	void setRoughness(float factor);
	


private:
	vec3 mColor;
	float mMetallic = 0;
	float mRoughness = 0.1;
};

#endif