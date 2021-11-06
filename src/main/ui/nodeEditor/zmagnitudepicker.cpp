#include <utility>

#include <utility>

//
// Created by lukas on 11/28/20.
//

#include <ui/zslider.h>
#include <utils/zsettingsstore.h>
#include "ui/zmagnitudepicker.h"

ZMagnitudePicker::ZMagnitudePicker(ZView *parent) : ZView(MAG_WIDTH, 70, parent) {

    setCornerRadius(10);
    setBackgroundColor(grey0);
    setElevation(1.0);

    // Slider example
    float min = -1.0;
    float max = 1.0;
    mSlider = new ZSlider("", min, max, 0.0, this);
    //mSlider->setShowLabel(false);
    mSlider->setMargin(vec4(3));
    mSlider->setYOffset(0);
    mSlider->setMaxHeight(20);
    mSlider->setGravity(bottomLeft);
    mSlider->setTextColor(black);

    mSlider->setOnSlide([this](ZView* view, float v, bool mouseUp){
        if (mListener != nullptr) {
            mListener(mSocketIndex, v, mIsInput, mSelectedMagnitude);
            view->invalidate();
        }
    });

    mName = new ZLabel("", this);
    mName->setYOffset(25);
    mName->setXOffset(5);
    mName->setTextColor(grey0.getTextColor());

    mRangeContainer = new ZView(fillParent, 30, this);
    mRangeContainer->setGravity(topLeft);
    mRangeContainer->setMargin(vec4(0));
}

void ZMagnitudePicker::setRadioButtonNames(vector<string> names) {
    mRadioButtonNames = std::move(names);
    initializeRadioButtons();
}

void ZMagnitudePicker::initializeRadioButtons() {
    int rangeWidth = MAG_WIDTH / mRadioButtonNames.size();
    int index = 0;
    for (const string &range : mRadioButtonNames) {

        if (index >= mRadioButtonLabels.size()) {
            ZLabel *label = new ZLabel(range, mRangeContainer);
            mRadioButtonLabels.push_back(label);
        }

        mRadioButtonLabels.at(index)->setText(range);
        mRadioButtonLabels.at(index)->setOutlineType(outline);
        mRadioButtonLabels.at(index)->setMargin(vec4(0));
        mRadioButtonLabels.at(index)->setTextColor(black);
        mRadioButtonLabels.at(index)->setLineWidth(2);
        mRadioButtonLabels.at(index)->setOutlineColor(grey);
        mRadioButtonLabels.at(index)->setCornerRadius(vec4(2,2,2,2));

        mRadioButtonLabels.at(index)->setMaxWidth(rangeWidth);
        mRadioButtonLabels.at(index)->setXOffset(rangeWidth * index);

        if (index == mSelectedMagnitude) {
            mRadioButtonLabels.at(index)->setBackgroundColor(ZSettings::get().getHighlightColor());
        } else {
            mRadioButtonLabels.at(index)->setBackgroundColor(getBackgroundColor());
        }
        index++;
    }

    int labelIndex = 0;
    for (ZLabel* label : mRadioButtonLabels) {
        label->setVisibility(labelIndex < mRadioButtonNames.size());
        labelIndex++;
    }
    onWindowChange(getWindowWidth(), getWindowHeight());
}

void ZMagnitudePicker::setValue(float value) {
    mSlider->setValue(value);
    invalidate();
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
                mListener(mSocketIndex, mSlider->getValue(), mIsInput, mSelectedMagnitude);
            }
        }
        setVisibility(false);

    }

    mSlider->setMouseDown(false);
    mSliderClicked = false;
}

void ZMagnitudePicker::onMouseEvent(int button, int action, int mods, int sx, int sy) {
    ZView::onMouseEvent(button, action, mods, sx, sy);

    if (action == GLFW_RELEASE ) {
        setVisibility(false);
    }
    mSlider->setMouseDown(false);
}

void ZMagnitudePicker::setValueChangedListener(function<void(int index, float value, bool isInput, int magIndex)> l) {
    mListener = std::move(l);
}

void ZMagnitudePicker::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) {
    ZView::onMouseDrag(absolute, start, delta, state, 0);

}

void ZMagnitudePicker::onCursorPosChange(double x, double y) {
    ZView::onCursorPosChange(x, y);

    if (getVisibility() && !mRadioButtonLabels.empty()) {
        if (getVisibility() && !mSliderClicked) {
            mSlider->onMouseEvent(GLFW_MOUSE_BUTTON_1, GLFW_PRESS, 0, 0, 0);
            mSliderClicked = true;
        }

        if (y > mRadioButtonLabels.at(0)->getTop() && y < mRadioButtonLabels.at(0)->getBottom() &&
            isMouseInBounds(this)) {
            int index = 0;
            for (ZLabel *label : mRadioButtonLabels) {
                if (isMouseInBounds(label)) {
                    selectMagnitude(index);
                }
                index++;
            }
        }
    }
}

void ZMagnitudePicker::setTitle(string name) {
    mName->setText(name);
}

void ZMagnitudePicker::selectMagnitude(int index) {
    for (ZLabel *label : mRadioButtonLabels) {
        label->setBackgroundColor(getBackgroundColor());
    }

    mRadioButtonLabels.at(index)->setBackgroundColor(ZSettings::get().getHighlightColor());
    mSlider->setMaxValue(mRanges.at(index));
    mSlider->setMinValue(-mRanges.at(index));
    mSlider->setIncrement(mRanges.at(index) / 10000.0);
    mSelectedMagnitude = index;

    if (mListener != nullptr) {
        mListener(mSocketIndex, mSlider->getValue(), mIsInput, mSelectedMagnitude);
    }
}

void ZMagnitudePicker::setSliderVisibility(bool visibility) {
    mSlider->setVisibility(visibility);
}
