#include "ui/zdropdown.h"

ZDropDown::ZDropDown(float maxWidth, float maxHeight, string items[], string resourcePath) : 
ZView(maxWidth, 500) {
	
	//setBackgroundColor(vec4(1,0,1,1));
	mButtonHeight = maxHeight;

    mTitle = new ZLabel(maxWidth, 21, "roboto/Roboto-Bold.ttf", resourcePath);
    mTitle->setText("Dropdown");
    mTitle->setTextColor(vec3(1));
    mTitle->setBackgroundColor(vec4(0.5));
    addSubView(mTitle);

    mDrawer = new ZLinearLayout(300, 300);
    mDrawer->setBackgroundColor(vec4(1));
    mDrawer->setOffset(0,30);
    mDrawer->setVisibility(false);
    addSubView(mDrawer);
}

void ZDropDown::draw() {
	ZView::draw();
}

void ZDropDown::setTitle(string text) {
	mTitle->setText(text);
}

void ZDropDown::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);

	if (action == GLFW_PRESS) {
	
	}

	if (action == GLFW_RELEASE) {
		if (y < mTitle->getBottom()) {
			mDrawer->setVisibility(!mDrawer->getVisibility());
		}
	}
}

void ZDropDown::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);

}

int ZDropDown::getMaxHeight() {
	ZView::getMaxHeight();
	return mButtonHeight;
}
	
void ZDropDown::onCursorPosChange(double x, double y)  {
	// ZView::onCursorPosChange(x, y);

	// if (mouseIsDown()) {
	// 	int deltaX =  x - getMouseDownX();
	// 	int deltaY = y - getMouseDownY();
	// }
}