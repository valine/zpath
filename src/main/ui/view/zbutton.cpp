#include <utility>
#include <functional>
#include <utility>
#include <mesh/zpath_constants.h>
#include <utils/zsettingsstore.h>
#include "ui/zbutton.h"

ZButton::ZButton(float maxWidth, float maxHeight) :
	ZView(maxWidth, maxHeight) {

	mLabel = new ZLabel(maxWidth, 18);
	mLabel->setOffset(10, (maxHeight - 16) / 2);
	mLabel->setText("Button");
	mLabel->setGravity(ZView::topLeft);
	mLabel->setTextColor(ZSettingsStore::getInstance().getHighlightTextColor());
	addSubView(mLabel);
}

ZButton::ZButton(string label, ZView *parent) :
    ZView(DEFAULT_WIDTH, DEFAULT_HEIGHT, parent){
    mLabel = new ZLabel(std::move(label), this);

    mLabel->setGravity(ZView::topLeft);
    mLabel->setTextColor(ZSettingsStore::getInstance().getBaseTextColor());
    mLabel->setOffset(10, (getMaxHeight() - 16) / 2);

    setMargin(BTN_MARGIN, BTN_MARGIN, BTN_MARGIN, BTN_MARGIN);
    setBackgroundColor(ZSettingsStore::getInstance().getBaseColor());
    setHighlighColor(ZSettingsStore::getInstance().getBaseColor() + vec4(0.1,0.1,0.1,0.0));
}

void ZButton::draw() {
	ZView::draw();
}

void ZButton::setOnClick(std::function<void(ZButton* sender)> onClick) {
    mOnClick = std::move(onClick);
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

			if (mOnClick != nullptr) {
			    mOnClick(this);
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
