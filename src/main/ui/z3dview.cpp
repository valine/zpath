#include "z3dview.h"

/**
	Z3DView constructor 
*/
Z3DView::Z3DView(float maxWidth, float maxHeight, ZRenderer *renderer) 
: ZView(maxWidth, maxHeight) {
    mRenderer = renderer;
    renderer->setParentView(this);
}

void Z3DView::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);

	if (mouseIsDown()) {
		// ZCamera* camera = mRenderer->getCamera();
		// camera->setPosition(vec3(3,3,0));
    }
}

void Z3DView::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);
}
	
void Z3DView::onCursorPosChange(double x, double y) {
	ZView::onCursorPosChange(x, y);
	ZCamera* camera = mRenderer->getCamera();
	
	if (mouseIsDown()) {
		int deltaX =  getLastX() - x;
		int deltaY = getLastY() - y;
	
		mRotationX += deltaX;
		mRotationY -= deltaY;

		vec3 position = glm::vec3(5,3,0);
		vec3 up = glm::vec3(0,1,0);

		mat4 rotationMatrix = mat4();
	    rotationMatrix = glm::rotate(rotationMatrix, (float) mRotationX / 100, glm::vec3(0, 1, 0));
		rotationMatrix = glm::rotate(rotationMatrix, (float) mRotationY / 100, glm::vec3(0, 0, 1));


		vec3 newPosition = vec3(rotationMatrix * vec4(position, 1.0));
		vec3 newUp = vec3(rotationMatrix * vec4(up, 1.0));
		camera->setPosition(newPosition);
		camera->setUp(newUp);
	}
}

void Z3DView::setRenderer(ZRenderer *renderer) {
    mRenderer = renderer;
    renderer->setParentView(this);
}

void Z3DView::draw() {
	int y = getParentView()->getHeight() + getParentView()->getTop() - getTop() - getHeight();
	glViewport(getLeft(),y,getWidth(),getHeight());

    mRenderer->draw();



}