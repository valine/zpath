#ifndef ZPOINTLIGHT_H
#define ZPOINTLIGHT_H
#include <vector>
#include "mesh/zmesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

class ZPointLight {

public:

	ZPointLight(vec3 color, vec3 position);
	vec3 getColor();
	vec3 getPosition();

	void setColor(vec3 color);
	void setPosition(vec3 position);
	
private:

	vec3 mPosition;
	vec3 mColor;
};

#endif