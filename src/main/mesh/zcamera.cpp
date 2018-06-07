#include "mesh/zcamera.h"
#include <iostream>
using namespace std;

ZCamera::ZCamera() {
	  mPosition = glm::vec3(15,0,0); // Camera is at (4,3,3), in World Space
	  mFront = glm::vec3(0,0,0); // and looks at the origin
	  mUp = glm::vec3(0,1,0);  // Head is up (set to 0,-1,0 to look upside-down)
}

void ZCamera::setViewMatrix(mat4 viewmatrix) {
	mViewMatrix = viewmatrix;
	mManualViewMode = true;
}

void ZCamera::setProjectionMatrix(mat4 projectionMatrix) {
	mProjectionMatrix = projectionMatrix;
	mManualProjectionMode = true;
}

void ZCamera::setPosition(vec3 position) {
	mPosition = position;
	mManualViewMode = false;
}

void ZCamera::setUp(vec3 up) {
	mUp = up;
	mManualViewMode = false;
}

void ZCamera::setFront(vec3 front) {
	mFront = front;
	mManualViewMode = false;
}

vec3 ZCamera::getFront() {
	return mFront;
}

vec3 ZCamera::getPosition() {
	if (mManualViewMode) {
		vec4 tmpTranslation = vec4(mTranslation.x, mTranslation.y, mTranslation.z, 1.0);
		vec4 viewTranslation = inverse(getViewMatrix()) * tmpTranslation;
		return vec3(viewTranslation.x, viewTranslation.y, viewTranslation.z);
	} else {
		vec4 tmpTranslation = vec4(mTranslation.x, mTranslation.y, mTranslation.z, 1.0);
		vec4 viewTranslation = tmpTranslation * getViewMatrix();
		vec3 tmpViewTranslation = vec3(viewTranslation.x, viewTranslation.y, viewTranslation.z);
		return mPosition - tmpViewTranslation;
	}

}

vec3 ZCamera::getUp() {
	return mUp;
}

void ZCamera::setWidth(int width) {
	mWidth = width;
	if (width == 0) {
		mWidth = 1;
	}
}

void ZCamera::setHeight(int height) {
	mHeight = height;
	if (height == 0) {
		mHeight = 1;
	}
}

int ZCamera::getWidth() {
	return mWidth;
}

int ZCamera::getHeight() {
	return mHeight;
}

void ZCamera::setTranslation(vec3 translation) {
	mTranslation = translation;
}

vec3 ZCamera::getTranslation() {
	return mTranslation;
}

mat4 ZCamera::getProjectionMatrix() {
	if (mManualProjectionMode) {
		return mProjectionMatrix;
	} else {
		mat4 projectionMatrix = perspective(glm::radians(mFocalLength), (float) mWidth / (float) mHeight, 0.1f, 1000.0f);
 		return projectionMatrix;
	}
}

void ZCamera::setFocalLength(float focalLength) {
	mFocalLength = focalLength;
	mManualProjectionMode = false;
}

float ZCamera::getFocalLength() {
	return mFocalLength;
}

mat4 ZCamera::getViewMatrix() {
	if (mManualViewMode) {
		return mViewMatrix;
	} else {
		mat4 mViewMatrix = glm::lookAt(
		    mPosition, // Camera is at (4,3,3), in World Space
		    mFront, // and looks at the origin
		    mUp  // Head is up (set to 0,-1,0 to look upside-down)
		);

		mat4 translationMat = mat4(1);
		translationMat = translate(translationMat, mTranslation);
		//mViewMatrix = glm::rotate(mViewMatrix, (float)glfwGetTime() / 5, glm::vec3(0, 1, 0)); // where x, y, z is axis of rotation (e.g. 0 1 0)
		return translationMat * mViewMatrix;
	}
}

bool ZCamera::isManualView() {
	return mManualViewMode;
}

void ZCamera::setBillboardTarget(vec3 target) {
	mBillboardTargetSet = true;
	mBillboardTarget = target;
}

vec3 ZCamera::getBillboardTarget() {
	if (mBillboardTargetSet) {
		return mBillboardTarget;
	} else {
		return getPosition();
	}
}
