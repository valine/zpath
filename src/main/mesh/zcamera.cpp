#include "zcamera.h"


ZCamera::ZCamera() {
	  mPosition = glm::vec3(5,7,0); // Camera is at (4,3,3), in World Space
	  mFront = glm::vec3(0,0,0); // and looks at the origin
	  mUp = glm::vec3(0,1,0);  // Head is up (set to 0,-1,0 to look upside-down)

}

void ZCamera::setPosition(vec3 position) {
	mPosition = position;
}

void ZCamera::setUp(vec3 up) {
	mUp = up;
}

void ZCamera::setFront(vec3 front) {
	mFront = front;
}

mat4 ZCamera::getViewMatrix() {

	mat4 mViewMatrix = glm::lookAt(
	    mPosition, // Camera is at (4,3,3), in World Space
	    mFront, // and looks at the origin
	    mUp  // Head is up (set to 0,-1,0 to look upside-down)
	);

	mViewMatrix = glm::rotate(mViewMatrix, (float)glfwGetTime() / 5, glm::vec3(0, 1, 0)); // where x, y, z is axis of rotation (e.g. 0 1 0)
	return mViewMatrix;
}