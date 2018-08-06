#ifndef ZCAMERA_H
#define ZCAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh/zobject.h"
using namespace glm;

class ZCamera : public ZObject {
	
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

	int getWidth();
	int getHeight();
	
	void setWidth(int width);
	void setHeight(int height);

	void setTranslation(vec3 translation);
	vec3 getTranslation();

	void setFocalLength(float focalLength);
	float getFocalLength();

	void setViewMatrix(mat4 viewmatrix);
	void setProjectionMatrix(mat4 projectionMatrix);
	void setBillboardTarget(vec3);
	vec3 getBillboardTarget();
	bool isManualView();

private:
	bool mManualViewMode = false;
	bool mManualProjectionMode = false;
	bool mBillboardTargetSet = false;
	
	vec3 mPosition = vec3(0);
	vec3 mFront = vec3(1);
	vec3 mUp = vec3(1);

	vec3 mBillboardTarget;

	vec3 mTranslation = vec3(0);

	int mWidth = 1;
	int mHeight = 1;

	mat4 mProjectionMatrix;
	mat4 mViewMatrix = mat4(1);

	float mFocalLength = 80.0;
};

#endif