#include "zbutton.h"


ZButton::ZButton(float maxWidth, float maxHeight, string resourcePath) : 
ZView(maxWidth, maxHeight) {
	

    ZLabel* label = new ZLabel(100, 21, "roboto/Roboto-Bold.ttf", resourcePath);
    label->setOffset(10,0);
    label->setMargin(0,0,0,0);
    label->setText("Properties");
    label->setTextColor(vec3(0.1,0.1,0.1));
    addSubView(label);

	//ZLabel* label = new ZLabel(maxWidth, maxHeight, "roboto/Roboto-Regular.ttf", resourcePath);
	//label->setMargin(0,0,0,0);
	//addSubView(label);
}

void ZButton::draw() {
	ZView::draw();

}

// void ZButton::onMouseEvent(int button, int action, int mods, int x, int y) {
// 	ZView::onMouseEvent(button, action, mods, x, y);

// 	// if (action == GLFW_PRESS) {
		
// 	// }

// 	// if (action == GLFW_RELEASE) {
		
// 	// }
// }

// void ZButton::onKeyPress(int key, int scancode, int action, int mods) {
// 	ZView::onKeyPress(key, scancode, action, mods);

// }
	
// void ZButton::onCursorPosChange(double x, double y)  {
// 	// ZView::onCursorPosChange(x, y);

// 	// if (mouseIsDown()) {
// 	// 	int deltaX =  x - getMouseDownX();
// 	// 	int deltaY = y - getMouseDownY();
// 	// }
// }