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

void Z3DView::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);
 	
 	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
 		cout<<getLeft()<<endl;
 		int index = mRenderer->getObjectIndexAtLocation(x - getLeft(), getTop() + getHeight() - 1 - y);
 		mRenderer->getScene()->setActiveObjectIndex(index);
 	} 
}

void Z3DView::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);

	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
 		ZScene* scene = mRenderer->getScene();
 		int activeObjectIndex = scene->getActiveObjectIndex();
 		if (activeObjectIndex != -1) {
 			mSpinRig->setTranslation(scene->getObjects().at(activeObjectIndex)->getOrigin());
 		} else {
 			mSpinRig->setTranslation(vec3(0));
 		}
 	}
}
	
void Z3DView::onCursorPosChange(double x, double y) {
	ZView::onCursorPosChange(x, y);
	int deltaX = getLastX() - x;
	int deltaY = getLastY() - y;
	if (middleMouseIsDown() && !shiftKeyPressed()) {
		//Orbit 

		mSpinRig->rotateBy(deltaX);
		mTiltRig->rotateBy(deltaY);

		mRotationX += deltaX;
		mRotationY -= deltaY;
	} else if (middleMouseIsDown() && shiftKeyPressed()) {
		// Pan
		double panSpeed = 0.05;
		mat4 cameraMatrix = ZRenderUtils::getModelMatrix(mRenderer->getCamera(), nullptr);
		vec4 rotationX = vec4(deltaX * panSpeed, deltaY * -panSpeed, 0.0, 0.0);
		rotationX = cameraMatrix * rotationX;
		mSpinRig->translateBy(dvec3(rotationX));
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
	
	mRenderer->getCamera()->translateBy(vec3(0,0,-y));
	updateCameraPosition();
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

	//ZView::draw();

	//glDisable(GL_DEPTH_TEST);

	//glDepthMask(false);

	//int yv = getWindowHeight() - getBottom();
	//glViewport(getLeft(),yv,getWidth(),getHeight());

    mRenderer->draw();
    //glDepthMask(true);

    //glEnable(GL_DEPTH_TEST);
}   