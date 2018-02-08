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
	if (mouseIsDown()) {
		int deltaX =  getLastX() - x;
		int deltaY = getLastY() - y;

		mRotationX += deltaX;
		mRotationY -= deltaY;

		updateCameraPosition();
	}
}

void Z3DView::onScrollChange(double x, double y) {
	ZView::onScrollChange(x, y);

	mOrbitAnchorPoint.x -= y;

	if (mOrbitAnchorPoint.x < 0) {
		mOrbitAnchorPoint.x = 0;
	}
	
	updateCameraPosition();
}

void Z3DView::setRenderer(ZRenderer *renderer) {
    mRenderer = renderer;
    renderer->setParentView(this);
}

void Z3DView::updateCameraPosition() {


	ZCamera* camera = mRenderer->getCamera();


	vec3 up = glm::vec3(0,1,0);

	mat4 rotationMatrix = mat4();
    rotationMatrix = glm::rotate(rotationMatrix, (float) mRotationX / 100, glm::vec3(0, 1, 0));
	rotationMatrix = glm::rotate(rotationMatrix, (float) mRotationY / 100, glm::vec3(0, 0, 1));

	vec3 newPosition = vec3(rotationMatrix * vec4(mOrbitAnchorPoint, 1.0));
	vec3 newUp = vec3(rotationMatrix * vec4(up, 1.0));
	camera->setPosition(newPosition);
	camera->setUp(newUp);
}

void Z3DView::draw() {

	ZView::draw();

	//glDisable(GL_DEPTH_TEST);

	//glDepthMask(false);

	int yv = getWindowHeight() - getBottom();
	glViewport(getLeft(),yv,getWidth(),getHeight());

    mRenderer->draw();

    //glDepthMask(true);

    //glEnable(GL_DEPTH_TEST);



}