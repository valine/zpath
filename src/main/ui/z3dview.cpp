#include "z3dview.h"

/**
	Z3DView constructor 
*/
Z3DView::Z3DView(float maxWidth, float maxHeight, ZRenderer *renderer) 
: ZView(maxWidth, maxHeight) {
    mRenderer = renderer;
    renderer->setParentView(this);
    renderer->init();
}

void Z3DView::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);
 	
 	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
 		int index = mRenderer->getObjectIndexAtLocation(getLeft() + x, getTop() + getHeight() - 1 - y);
 		// vector<ZObject*> objects = mRenderer->getScene()->getObjects();

 		// for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
			// ZObject *object = (*it);
			// object->setSelected(false);
 		// }

 		mRenderer->getScene()->setActiveObjectIndex(index);
 	}

}

void Z3DView::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);
}
	
void Z3DView::onCursorPosChange(double x, double y) {
	ZView::onCursorPosChange(x, y);
	int deltaX =  getLastX() - x;
	int deltaY = getLastY() - y;
	if (middleMouseIsDown() && !shiftKeyPressed()) {
		//Orbit 
		mRotationX += deltaX;
		mRotationY -= deltaY;
	} else if (middleMouseIsDown() && shiftKeyPressed()) {
		// Pan
		mTranslation.y += deltaX / 1;
		mTranslation.z += deltaY / 1;
	}

	updateCameraPosition();
}

void Z3DView::onScrollChange(double x, double y) {
	ZView::onScrollChange(x, y);
	if (y > 0) {
		mOrbitAnchorPoint.x -= mOrbitAnchorPoint.x / 5;
	} else {
		if (mOrbitAnchorPoint.x < 1) {
			mOrbitAnchorPoint.x += mOrbitAnchorPoint.x / 2;
		} else {
			mOrbitAnchorPoint.x += mOrbitAnchorPoint.x / 5;
		}
	}

	if (mOrbitAnchorPoint.x < 0) {
		mOrbitAnchorPoint.x = 0.1;
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
	vec3 newFront = camera->getFront();

	// newFront.y = mTranslation.y;
	// newFront.z = mTranslation.z;

	// newPosition.y += mTranslation.y;
	// newPosition.z += mTranslation.z;

	// newUp.y += mTranslation.y;
	// newUp.z += mTranslation.z;

	camera->setPosition(newPosition);
	camera->setUp(newUp);
	camera->setFront(newFront);
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