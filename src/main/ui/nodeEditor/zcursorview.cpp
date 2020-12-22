//
// Created by lukas on 12/21/20.
//

#include <ui/zlineview.h>
#include "ui/zcursorview.h"

ZCursorView::ZCursorView(ZView *parent) : ZView(fillParent, fillParent, parent) {

    mHorizontal = new ZLineView(vec2(0, getHeight() / 2), vec2(getWidth(), getHeight() / 2), this);
    mHorizontal->setOutlineColor(red);

    mVertical = new ZLineView(vec2(getWidth() / 2, 0), vec2(getWidth() / 2, getHeight()), this);
    mVertical->setOutlineColor(red);
}

void ZCursorView::setPosition(vec2 position) {
    mVertical->setPoints(vec2(position.x, 0), vec2(position.x, getHeight()));
    mHorizontal->setPoints(vec2(0, position.y), vec2(getWidth(), position.y));
    invalidate();
}
