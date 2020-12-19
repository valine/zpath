//
// Created by lukas on 11/28/20.
//

#include <ui/zslider.h>
#include "ui/zmagnitudepicker.h"

ZMagnitudePicker::ZMagnitudePicker(ZView *parent) : ZView(300, 70, parent) {

    setBackgroundColor(white);

    setDrawWire(WireType::outline);
    setOutlineColor(black);

    // Slider example
    float min = -1.0;
    float max = 1.0;
    mSlider = new ZSlider("", min, max, 0.0, this);
    mSlider->setLineColor(grey);
    mSlider->setOnSlide([this](ZView* view, float v, bool b){
        if (mListener != nullptr) {
            mListener(v);
        }
    });
}

void ZMagnitudePicker::onGlobalMouseUp(int key) {
    ZView::onGlobalMouseUp(0);

    if (!isMouseInBounds(this)) {
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

void ZMagnitudePicker::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state) {
    ZView::onMouseDrag(absolute, start, delta, state);

}

void ZMagnitudePicker::onCursorPosChange(double x, double y) {
    ZView::onCursorPosChange(x, y);

    if (isMouseInBounds(mSlider) && getVisibility()) {
        mSlider->onMouseEvent(GLFW_MOUSE_BUTTON_1, GLFW_PRESS, 0, 0,0);
    }



}
