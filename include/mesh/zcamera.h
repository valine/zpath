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
	vec3 getFront();
	mat4 getViewMatrix();
	mat4 getProjectionMatrix();

	void setWidth(int width);
	void setHeight(int height);

	void setTranslation(vec3 translation);
	vec3 getTranslation();

	void setFocalLength(float focalLength);
	float getFocalLength();

private:
	vec3 mPosition;
	vec3 mFront;
	vec3 mUp;

	vec3 mTranslation = vec3(0,0,0);

	int mWidth;
	int mHeight;

	float mFocalLength = 80.0;
};

#endif