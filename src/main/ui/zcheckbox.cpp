#include "ui/zcheckbox.h"

#include <utility>

ZCheckbox::ZCheckbox(float maxWidth, float maxHeight, string resourcePath) :
	ZView(maxWidth, maxHeight) {

	mBox = new ZView(maxHeight, maxHeight);
	mBox->setBackgroundColor(vec4(1));
	addSubView(mBox);

	mCheck = new ZView(maxHeight, maxHeight);
	mCheck->setBackgroundColor(vec4(0.5));
	mCheck->setVisibility(mIsChecked);
	addSubView(mCheck);

	mLabel = new ZLabel(maxWidth, 18);
	mLabel->setOffset(maxHeight + 10, 1);
	mLabel->setText("Checkbox");
	mLabel->setGravity(ZView::topLeft);
	mLabel->setTextColor(vec3(0, 0, 0));
	addSubView(mLabel);

	//ZLabel* label = new ZLabel(maxWidth, maxHeight, "roboto/Roboto-Regular.ttf", resourcePath);
	//label->setMargin(0,0,0,0);
	//addSubView(label);
}

ZCheckbox::ZCheckbox(string label, string resourcePath, ZView* parent) :
ZView(CB_WIDTH, CB_HEIGHT) {
    mBox = new ZView(CB_HEIGHT, CB_HEIGHT);
    mBox->setBackgroundColor(vec4(1));
    addSubView(mBox);

    mCheck = new ZView(CB_HEIGHT, CB_HEIGHT);
    mCheck->setBackgroundColor(vec4(0.5));
    mCheck->setVisibility(mIsChecked);
    addSubView(mCheck);

    mLabel = new ZLabel(CB_WIDTH, 18);
    mLabel->setOffset(CB_HEIGHT + 10, 1);
    mLabel->setText(label);
    mLabel->setGravity(ZView::topLeft);
    mLabel->setTextColor(vec3(0, 0, 0));
    addSubView(mLabel);
    
    setCheckBackground(new ZTexture(resourcePath +
                                                         "resources/icons/check.png"));
    setBoxBackground(new ZTexture(resourcePath +
                                                       "resources/icons/box.png"));

    setMargin(5,5,5,5);
    setCheckColor(vec4(0.5,0.5,0.5,1.0));
    setVisibility(true);
    parent->addSubView(this);

}

void ZCheckbox::setChecked(bool checked) {
	mIsChecked = checked;

	mCheck->setVisibility(mIsChecked);
	if (mListener != nullptr) {
		mListener->onCheckChange(this, mIsChecked);
	}
}

void ZCheckbox::setCheckColor(vec4 color) {
    mBox->setBackgroundColor(color);
    mCheck->setBackgroundColor(color);
}

void ZCheckbox::setBoxBackground(ZTexture* t) {
	mBox->setBackgroundImage(t);
}

void ZCheckbox::setCheckBackground(ZTexture* t) {
	mCheck->setBackgroundImage(t);
}

void ZCheckbox::draw() {
	ZView::draw();

}
void ZCheckbox::setText(string text) {
	mLabel->setText(text);
}

void ZCheckbox::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);
	vec4 highlight = vec4(0.2, 0.2, 0.2, 0.2);

	if (action == GLFW_PRESS) {
		setBackgroundColor(getBackgroundColor() + highlight);
	}

	if (action == GLFW_RELEASE) {
		setBackgroundColor(getBackgroundColor() - highlight);
		mIsChecked = !mIsChecked;

		mCheck->setVisibility(mIsChecked);
		if (mListener != nullptr) {
			mListener->onCheckChange(this, mIsChecked);
		}

		if (mClickListener != nullptr) {
		    mClickListener(this, mIsChecked);
		}
	}
}

void ZCheckbox::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);

}

void ZCheckbox::onCursorPosChange(double x, double y)  {
	// ZView::onCursorPosChange(x, y);

	// if (mouseIsDown()) {
	// 	int deltaX =  x - getMouseDownX();
	// 	int deltaY = y - getMouseDownY();
	// }
}

void ZCheckbox::setListener(ZCheckboxListener* l) {
	mListener = l;
}

void ZCheckbox::setOnClick(function<void(ZView*, bool)> onClick) {
    mClickListener = onClick;
}
