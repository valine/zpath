#include <utility>
#include <functional>
#include <utility>
#include <mesh/zpath_constants.h>
#include <utils/zsettingsstore.h>
#include "ui/zbutton.h"

ZButton::ZButton(float maxWidth, float maxHeight) :
	ZView(maxWidth, maxHeight) {

	mLabel = new ZLabel(maxWidth, 18);
	mLabel->setOffset(10, 0);
	mLabel->setText("Button");
	mLabel->setGravity(ZView::topLeft);
	addSubView(mLabel);
}

ZButton::ZButton(string label, ZView *parent) :
    ZView(DEFAULT_WIDTH, DEFAULT_HEIGHT, parent){
    mLabel = new ZLabel(std::move(label), this);

    mLabel->setGravity(ZView::topLeft);
    mLabel->setOffset(10, (getHeight() - mLabel->getLineHeight()) / 2);

    setCornerRadius(   12.5);
    setBackgroundColor(highlight);

    setMargin(BTN_MARGIN, BTN_MARGIN, BTN_MARGIN, BTN_MARGIN);
    setHighlighColor(highlight + vec4(0.1,0.1,0.1,0.0));
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

	vec4 highlight = vec4(0.1, 0.1, 0.1, 0);

	if (action == GLFW_PRESS) {
		setBackgroundColor(getBackgroundColor() - highlight);
		showHighlight(false);
		mWasPressed = true;
		invalidate();
	}

	if (action == GLFW_RELEASE) {
		if (mWasPressed || mClickMode == upAndDown) {

		    if (mWasPressed) {
                setBackgroundColor(getBackgroundColor() + highlight);
            }

            if (mOnClickSimple != nullptr) {
                mOnClickSimple();
            }
			if (mListener != nullptr) {
				mListener->onClick(this);
			}
		}

		mWasPressed = false;
		invalidate();
	}

}

ZLabel* ZButton::getLabel(){
    return mLabel;
}

void ZButton::computeBounds() {
    ZView::computeBounds();
}
void ZButton::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);
}

void ZButton::onCursorPosChange(double x, double y)  {
	 ZView::onCursorPosChange(x, y);
}

void ZButton::onMouseOver() {
    ZView::onMouseOver();
    showHighlight(true);
}

void ZButton::onMouseLeave() {
    ZView::onMouseLeave();
    showHighlight(false);
}

void ZButton::setOnClickListener(ZOnClickListener* l) {
	mListener = l;
}

void ZButton::setClickMode(ClickMode clickMode) {
    mClickMode = clickMode;
}

void ZButton::setOnClick(std::function<void()> onClick) {
    mOnClickSimple = std::move(onClick);
}

void ZButton::setBackgroundColor(ZColor color) {
    ZView::setBackgroundColor(color);

    vec4 color4 = color.get(mColorMode);
    vec3 color3 = vec3(color4.r,color4.g,color4.b);
    if (glm::length(color3) < 0.8 && color4.a != 0) {
        mLabel->setTextColor(vec4(1));
    } else {
        mLabel->setTextColor(vec4(0,0,0,1));
    }
}

void ZButton::setOnClick(std::function<void(ZView *)> onClick) {
    ZView::setOnClick(onClick);
}

void ZButton::onSizeChange() {
    ZView::onSizeChange();
    mLabel->setOffset(10, (getHeight() - mLabel->getLineHeight()) / 2);
}
