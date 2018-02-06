#include "zbutton.h"


ZButton::ZButton(float maxWidth, float maxHeight) : 
ZView(maxWidth, maxHeight) {
	

}

void ZButton::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);

	if (action == GLFW_PRESS) {
		
	}

	if (action == GLFW_RELEASE) {
		
	}
}

void ZButton::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);

}
	
void ZButton::onCursorPosChange(double x, double y)  {
	ZView::onCursorPosChange(x, y);

	if (mouseIsDown()) {
		int deltaX =  x - getMouseDownX();
		int deltaY = y - getMouseDownY();
	}
}