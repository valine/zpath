#include "z3dview.h"

Z3DView::Z3DView(float maxWidth, float maxHeight, int debug) 
: ZView(maxWidth, maxHeight) {
	mDebug = debug;
}

void Z3DView::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);

	
}

void Z3DView::onKeyPress(int key, int scancode, int action, int mods) {

}
	
void Z3DView::onCursorPosChange(double x, double y) {
	if (mouseIsDown()) {
	
	}
}

void Z3DView::draw() {
	glViewport(getLeft(),getRight(),getTop(),getBottom());
}

void Z3DView::setShader(ZShader* shader) {

	
}