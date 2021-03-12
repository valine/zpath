//
// Created by lukas on 7/27/20.
//

#include <utils/zsettingsstore.h>
#include "ui/zlineview.h"

ZLineView::ZLineView(vec2 point1, vec2 point2, ZView *parent): ZView(fillParent, fillParent, parent)  {
    mPoint1 = point1;
    mPoint2 = point2;

    init();
}

void ZLineView::init() {
    setBackgroundColor(ZSettingsStore::getInstance().getHighlightColor());
    setOutlineType(full);
}

void ZLineView::setPoints(vec2 point1, vec2 point2) {
    mPoint1 = point1;
    mPoint2 = point2;
    computeBounds();
}

void ZLineView::computeBounds() {
    ZView::computeBounds();
    calculateLeft();
    calculateTop();
    calculateBottom();
    calculateRight();

    float* vertices = getVertices();

    vertices[0] = mPoint1.x;
    vertices[1] = mPoint1.y;

    vertices[4] = mPoint2.x;
    vertices[5] = mPoint2.y;

    vertices[8] =  vertices[0];
    vertices[9] =  vertices[1];

    vertices[12] = vertices[4];
    vertices[13] =  vertices[5];

    invalidate();
}