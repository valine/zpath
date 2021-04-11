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

void ZCamera::setNearClipping(float clipping) {
	mNearClipping = clipping;
}

float ZCamera::getNearClipping() {
	return mNearClipping;
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

void ZCamera::setUsePerspective(bool use) {
	mPerspective = use;
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

bool ZCamera::isPerspective() {
	return mPerspective;
}

mat4 ZCamera::getProjectionMatrix() {
	if (mManualProjectionMode) {
		return mProjectionMatrix;
	} else {
		if (!mPerspective) {
			float scale = getTranslation().z / 2.0;
			float aspect = (float) mHeight / (float) mWidth;
			mat4 projectionMatrix = ortho(-1.0f * scale, 1.0f * scale, -aspect * scale, aspect * scale, mNearClipping, 1000.0f);
	 		return projectionMatrix;
		} else {
			mat4 projectionMatrix = perspective(glm::radians(mFocalLength), (float) mWidth / (float) mHeight, mNearClipping, 1000.0f);
	 		return projectionMatrix;
 		}
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
		    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

		mViewMatrix = mat4(1);

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
