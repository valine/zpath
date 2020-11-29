//
// Created by lukas on 11/28/20.
//

#include "ui/zmagnitudepicker.h"

ZMagnitudePicker::ZMagnitudePicker(ZView *parent) : ZView(200, 200, parent) {

    setBackgroundColor(faded);
}

void ZMagnitudePicker::onGlobalMouseUp() {
    ZView::onGlobalMouseUp();

    if (!isMouseInBounds(this)) {
        setVisibility(false);
    }
}
