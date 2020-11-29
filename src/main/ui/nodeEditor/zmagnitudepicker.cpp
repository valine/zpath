//
// Created by lukas on 11/28/20.
//

#include "ui/zmagnitudepicker.h"

ZMagnitudePicker::ZMagnitudePicker(ZView *parent) : ZView(300, 70, parent) {

    setBackgroundColor(faded);
}

void ZMagnitudePicker::onGlobalMouseUp() {
    ZView::onGlobalMouseUp();

    if (!isMouseInBounds(this)) {
        setVisibility(false);
    }
}

void ZMagnitudePicker::onMouseEvent(int button, int action, int mods, int sx, int sy) {
    ZView::onMouseEvent(button, action, mods, sx, sy);

    if (action == GLFW_RELEASE ) {
       setVisibility(false);
    }
}