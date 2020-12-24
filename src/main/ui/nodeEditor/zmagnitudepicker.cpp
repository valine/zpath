//
// Created by lukas on 11/28/20.
//

#include <ui/zslider.h>
#include <utils/zsettingsstore.h>
#include "ui/zmagnitudepicker.h"

ZMagnitudePicker::ZMagnitudePicker(ZView *parent) : ZView(MAG_WIDTH, 70, parent) {

    setBackgroundColor(white);

    setOutlineType(WireType::outline);
    setOutlineColor(black);

    // Slider example
    float min = -1.0;
    float max = 1.0;
    mSlider = new ZSlider("", min, max, 0.0, this);
    mSlider->setShowLabel(false);
    mSlider->setMargin(vec4(0));
    mSlider->setYOffset(0);
    mSlider->setMaxHeight(20);
    mSlider->setGravity(bottomLeft);
    mSlider->setLineColor(grey);
    mSlider->setOnSlide([this](ZView* view, float v, bool b){
        if (mListener != nullptr) {
            mListener(v);
            view->invalidate();
        }
    });

    mRangeContainer = new ZView(fillParent, 30, this);
    mRangeContainer->setGravity(topLeft);
    mRangeContainer->setMargin(vec4(0));

    int rangeWidth = MAG_WIDTH / mRangeStrings.size();
    int index = 0;
    for (const string& range : mRangeStrings) {
        ZLabel* label = new ZLabel(range, mRangeContainer);
        label->setMaxWidth(rangeWidth);
        label->setXOffset(rangeWidth * index);
        label->setOutlineType(WireType::outline);
        label->setMargin(vec4(0));
        mRangeLabels.push_back(label);

        if (mRanges.at(index) == 1) {
            label->setBackgroundColor(ZSettingsStore::get().getHighlightColor());
        } else {
            label->setBackgroundColor(getBackgroundColor());
        }
        index++;
    }
}

void ZMagnitudePicker::setShowAbove(bool above) {
    if (above) {
        mRangeContainer->setYOffset(0);
        mSlider->setGravity(bottomLeft);
    } else {
        mRangeContainer->setYOffset(getHeight() - 20);
        mSlider->setGravity(topLeft);
    }
}

void ZMagnitudePicker::onGlobalMouseUp(int key) {
    ZView::onGlobalMouseUp(0);

    if (!isMouseInBounds(this)) {
        if (getVisibility()) {
            if (mListener != nullptr) {
                mListener(mSlider->getValue());
            }
        }
        setVisibility(false);

    }

    mSlider->setMouseDown(false);
}

bool ZMagnitudePicker::onMouseEvent(int button, int action, int mods, int sx, int sy) {
    ZView::onMouseEvent(button, action, mods, sx, sy);

    if (action == GLFW_RELEASE ) {
        setVisibility(false);
    }
    mSlider->setMouseDown(false);
    return false;
}

void ZMagnitudePicker::setValueChangedListener(function<void(float value)> l) {
    mListener = l;
}

void ZMagnitudePicker::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) {
    ZView::onMouseDrag(absolute, start, delta, state, 0);

}

void ZMagnitudePicker::onCursorPosChange(double x, double y) {
    ZView::onCursorPosChange(x, y);

    if (isMouseInBounds(mSlider) && getVisibility()) {
        mSlider->onMouseEvent(GLFW_MOUSE_BUTTON_1, GLFW_PRESS, 0, 0,0);
    }

    if (y > mRangeLabels.at(0)->getTop() && y < mRangeLabels.at(0)->getBottom() && isMouseInBounds(this)) {
        int index = 0;
        for (ZLabel *label : mRangeLabels) {
            if (isMouseInBounds(label)) {
                label->setBackgroundColor(ZSettingsStore::get().getHighlightColor());
                mSlider->setMaxValue(mRanges.at(index));
                mSlider->setMinValue(-mRanges.at(index));
                mSlider->setIncrement(mRanges.at(index) / 1000.0);
            } else {
                label->setBackgroundColor(getBackgroundColor());
            }

            index++;
        }
    }


}
