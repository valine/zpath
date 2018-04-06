#ifndef ZCAMERA_H
#define ZCAMERA_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
using namespace glm;

class ZCamera {
	
public:
	ZCamera();

	void setPosition(vec3 position);
	void setUp(vec3 up);
	void setFront(vec3 front);

	vec3 getPosition();
	vec3 getUp();
	mat4 getViewMatrix();

private:
	vec3 mPosition;
	vec3 mFront;
	vec3 mUp;
};

#endif