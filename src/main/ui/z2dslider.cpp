#include <functional>
#include <utils/zsettingsstore.h>
#include "ui/z2dslider.h"

Z2DSlider::Z2DSlider(float maxWidth, float maxHeight, vec2 min, vec2 max, ZView *parent) :
ZView(maxWidth, maxHeight){
    mLabel = new ZLabel(ZView::fillParent, 18);
    mLabel->setOffset(0,0);
    mLabel->setGravity(ZView::topLeft);
    mLabel->setTextColor(ZSettingsStore::getInstance().getHighlightTextColor());
    addSubView(mLabel);

    mLineH = new ZView(ZView::fillParent, LINE_WIDTH, this);
    mLineH->setOffset(0, getHeight() / 2);
    mLineH->setBackgroundColor(ZSettingsStore::getInstance().getHighlightColor());
    mLineH->setGravity(ZView::bottomLeft);
  
    mLineV = new ZView(LINE_WIDTH, ZView::fillParent, this);
    mLineV->setOffset(getWidth() / 2, 0);
    mLineV->setBackgroundColor(ZSettingsStore::getInstance().getHighlightColor());
    mLineV->setGravity(ZView::bottomLeft);

    setMargin(vec4(5));
    setMaxValue(max);
    setMinValue(min);

    parent->addSubView(this);
}

void Z2DSlider::setTextColor(vec3 color) {
    mLabel->setTextColor(color);
}

void Z2DSlider::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);
 	if (action == GLFW_RELEASE) {

 	    // todo value
 	    float value = 0.0;

//        if (mSlideListener != nullptr) {
//            mSlideListener(this, value, true);
//        }
 	}
}

void Z2DSlider::onCursorPosChange(double x, double y) {
	ZView::onCursorPosChange(x, y);
	if (mouseIsDown()) {

        int deltaX = getLastX() - x;
        int deltaY = y - getLastY();

        int currentOffsetX = mLineV->getOffsetX();
        int newOffsetX = currentOffsetX - deltaX;

        int currentOffsetY = mLineH->getOffsetY();
        int newOffsetY = currentOffsetY - deltaY;

        if (newOffsetX < 0) {
            newOffsetX = 0;
        } else if (newOffsetX > getWidth()) {
            newOffsetX = getWidth();
        }

        if (newOffsetY < 0) {
            newOffsetY = 0;
        } else if (newOffsetY > getHeight()) {
            newOffsetY = getHeight();
        }

        mLineH->setOffset(0, newOffsetY);
        mLineV->setOffset(newOffsetX, 0);

        valueChanged(newOffsetX, newOffsetY);
    }
}

void Z2DSlider::valueChanged(float x, float y) {
    float maxOffsetX = (float) (getWidth());
    float factorX = (float) x / maxOffsetX;
    float valueX = ((mMaxValue.x - mMinValue.x) * factorX) + mMinValue.x;
    float incValueX = roundf(valueX / mIncrement) * mIncrement;

    float maxOffsetY = (float) (getHeight());
    float factorY = (float) y / maxOffsetY;
    float valueY = ((mMaxValue.y - mMinValue.y) * factorY) + mMinValue.y;
    float incValueY = roundf(valueY / mIncrement) * mIncrement;

    auto format = "%.2f";
    auto sizeX = snprintf(nullptr, 0, format, incValueX);
    auto sizeY = snprintf(nullptr, 0, format, incValueX);
    string output(sizeX, '\0');
    sprintf(&output[0], format, incValueX);

    string outputY(sizeY, '\0');
    sprintf(&outputY[0], format, incValueY);

    mValue = vec2(incValueX, incValueY);
    if (mLabel != nullptr) {
        mLabel->setText(mTitle + " " + output + " " + outputY);
    }

    if (mSlideListener != nullptr) {
        mSlideListener(this, mValue, false);
    }

}

vec2 Z2DSlider::getValue() {
    return mValue;
}

void Z2DSlider::setValue(vec2 value) {

}

void Z2DSlider::setMaxValue(vec2 max) {
	mMaxValue = max;
}

void Z2DSlider::setMinValue(vec2 min) {
	mMinValue = min;
}

void Z2DSlider::setIncrement(float increment) {
    mIncrement = increment;
}

void Z2DSlider::setOnSlide(std::function<void(ZView*, vec2, bool)> onSlide) {
    mSlideListener = onSlide;
}


