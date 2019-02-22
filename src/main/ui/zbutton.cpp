#include "ui/zbutton.h"

ZButton::ZButton(float maxWidth, float maxHeight, string resourcePath) :
	ZView(maxWidth, maxHeight) {


	mLabel = new ZLabel(maxWidth, 18, "roboto/Roboto-Medium.ttf", resourcePath);
	mLabel->setOffset(10, (maxHeight - 16) / 2);
	mLabel->setText("Button");
	mLabel->setGravity(ZView::topLeft);
	mLabel->setTextColor(vec3(1, 1, 1));
	addSubView(mLabel);

	//ZLabel* label = new ZLabel(maxWidth, maxHeight, "roboto/Roboto-Regular.ttf", resourcePath);
	//label->setMargin(0,0,0,0);
	//addSubView(label);
}

void ZButton::draw() {
	ZView::draw();

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
	}

	if (action == GLFW_RELEASE) {
		setBackgroundColor(getBackgroundColor() - highlight);

		if (mListener != nullptr) {
			mListener->onClick(this);
		}
	}
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