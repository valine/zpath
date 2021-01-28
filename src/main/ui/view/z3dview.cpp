#include "ui/z3dview.h"

/**
	Z3DView constructor 
*/
Z3DView::Z3DView(float maxWidth, float maxHeight, ZRenderer *renderer) 
: ZView(maxWidth, maxHeight) {
    mRenderer = renderer;
    mRenderer->setRenderToTexture(false);
    renderer->setParentView(this);
    renderer->init();

    mTiltRig = new ZObject();
    mSpinRig = new ZObject();

    mTiltRig->setParent(mSpinRig);
    renderer->getCamera()->setParent(mTiltRig);

    mTiltRig->setRotation(vec3(1,0,0));
    mSpinRig->setRotation(vec3(0,1,0));
}

bool Z3DView::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);
 	if ((button == GLFW_MOUSE_BUTTON_1 || button == GLFW_MOUSE_BUTTON_2) && action == GLFW_RELEASE && !altKeyPressed()) {
 		int index = mRenderer->getObjectIndexAtLocation(x - getLeft(), getTop() + getHeight() - 1 - y);
 		mRenderer->getScene()->setActiveObjectIndex(index);
 		invalidate();
 	}

 	return true;
}

void Z3DView::setTilt(float angle) {
	mTiltRig->setRotationAngle(angle);
}

void Z3DView::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
 		ZScene* scene = mRenderer->getScene();
 		int activeObjectIndex = scene->getActiveObjectIndex();
 		if (activeObjectIndex != -1 && activeObjectIndex < scene->getObjects().size()) {
 			ZObject* object = scene->getObjects().at(activeObjectIndex);
 			mSpinRig->setTranslation(ZRenderUtils::getModelMatrix(object, nullptr) * vec4(object->getOrigin(), 1.0));
 		} else {
 			mSpinRig->setTranslation(vec3(0));
 		}
 	}
}

void Z3DView::setWireMode(bool wiremode) {
	mRenderer->setWireMode(wiremode);
}
	
void Z3DView::onCursorPosChange(double x, double y) {
	ZView::onCursorPosChange(x, y);
	int deltaX = getLastX() - x;
	int deltaY = getLastY() - y;
	if ((middleMouseIsDown() || (mouseIsDown() && altKeyPressed())) && !shiftKeyPressed()) {
		//Orbit 

		mSpinRig->rotateBy(deltaX);
		mTiltRig->rotateBy(deltaY);

		mRotationX += deltaX / 2;
		mRotationY -= deltaY / 2;
		invalidate();
	} else if ((middleMouseIsDown() || (mouseIsDown() && altKeyPressed()))  && shiftKeyPressed()) {
		// Pan
		double panSpeed = 0.02;
		mat4 cameraMatrix = ZRenderUtils::getModelMatrix(mRenderer->getCamera(), nullptr);
		vec4 rotation = vec4(deltaX * panSpeed, deltaY * -panSpeed, 0.0, 0.0);
		rotation = cameraMatrix * rotation;
		mSpinRig->translateBy(dvec3(rotation));
		invalidate();
	}

	updateCameraPosition();
}

void Z3DView::onWindowChange(int width, int height) {
	ZView::onWindowChange(width, height);
	mRenderer->getCamera()->setWidth(getWidth());
	mRenderer->getCamera()->setHeight(getHeight());
	mRenderer->recreateBuffers();
}

void Z3DView::onScrollChange(double x, double y) {
	ZView::onScrollChange(x, y);
	if (shiftKeyPressed()) {
		mRenderer->getCamera()->translateBy(vec3(0,0, -y / 16));
	} else {
		mRenderer->getCamera()->translateBy(vec3(0,0,-y / 4));
	}

}

void Z3DView::onExit() {
	ZView::onExit();
	mRenderer->onExit();
}

ZRenderer* Z3DView::getRenderer() {
	return mRenderer;
}

void Z3DView::setRenderer(ZRenderer *renderer) {
    mRenderer = renderer;
    renderer->setParentView(this);
}

void Z3DView::updateCameraPosition() {

}

ZScene* Z3DView::getScene() {
	return mRenderer->getScene();
}

void Z3DView::draw() {
	glDisable(GL_DEPTH_TEST);
	//glDepthMask(true);

	//int yv = getWindowHeight() - getBottom();
	//glViewport(getLeft(),yv,getWidth(),getHeight());
	if (getVisibility()) {
    	mRenderer->draw();
	}

    for (ZView* view : getSubViews()) {
        view->draw();
    }

   // ZView::draw();
    //glDepthMask(true);
    //glEnable(GL_DEPTH_TEST);
}   