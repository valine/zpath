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
		ZCamera* camera = mRenderer->getCamera();
		camera->setPosition(vec3(3,3,0));
    }
}

void Z3DView::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);
}
	
void Z3DView::onCursorPosChange(double x, double y) {

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