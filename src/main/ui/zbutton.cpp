#include <functional>
#include "ui/zbutton.h"

ZButton::ZButton(float maxWidth, float maxHeight, string resourcePath) :
	ZView(maxWidth, maxHeight) {


	mLabel = new ZLabel(maxWidth, 18);
	mLabel->setOffset(10, (maxHeight - 16) / 2);
	mLabel->setText("Button");
	mLabel->setGravity(ZView::topLeft);
	mLabel->setTextColor(vec3(1, 1, 1));
	addSubView(mLabel);
}

void ZButton::draw() {
	ZView::draw();

}

void ZButton::setOnClick(std::function<void()> onClick) {
    mOnClick = onClick;
}

string ZButton::getText() {
	return mLabel->getText();
}

void ZButton::setText(string text) {
	mLabel->setText(text);
}

void ZButton::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);
	vec4 highlight = vec4(0.2, 0.2, 0.2, 0);

	if (action == GLFW_PRESS) {
		setBackgroundColor(getBackgroundColor() + highlight);
		mWasPressed = true;
	}

	if (action == GLFW_RELEASE) {
		if (mWasPressed) {
			setBackgroundColor(getBackgroundColor() - highlight);

			if (mOnClick != nullptr) {
			    mOnClick();
			}
			if (mListener != nullptr) {
				mListener->onClick(this);
			}
		}

		mWasPressed = false;
	}
}

void ZButton::computeBounds(int windowHeight, int maxWidth) {
	mLabel->setOffset(10, (getHeight() - 16) / 2);
	ZView::computeBounds(windowHeight, maxWidth);
}
void ZButton::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);
}

void ZButton::onCursorPosChange(double x, double y)  {
	// ZView::onCursorPosChange(x, y);

	// if (mouseIsDown()) {
	// 	int deltaX =  x - getMouseDownX();
	// 	int deltaY = y - getMouseDownY();
	// }
}

void ZButton::setOnClickListener(ZOnClickListener* l) {
	mListener = l;
}