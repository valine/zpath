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
    setOutlineType(none);
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

    float angle = atan2((mPoint2.y - mPoint1.y), (mPoint2.x - mPoint1.x));
    float lineWidth = 0.75 * mDP;
    float xOffset = sin(angle) * lineWidth;
    float yOffset = cos(angle) * lineWidth;

    vertices[0] = mPoint1.x + xOffset;
    vertices[1] = mPoint1.y - yOffset;

    vertices[4] = mPoint2.x + xOffset;
    vertices[5] = mPoint2.y - yOffset;;

    vertices[8] =  mPoint1.x - xOffset;
    vertices[9] =  mPoint1.y + yOffset;

    vertices[12] = mPoint2.x - xOffset;
    vertices[13] = mPoint2.y + yOffset;


    invalidate();
}