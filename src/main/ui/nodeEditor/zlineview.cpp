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
    setBackgroundColor(ZSettings::getInstance().getHighlightColor());

    if (mDP > 1.0) {
        setOutlineType(none);
    } else {
        setOutlineType(full);
    }

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

    // If dp is higher than 1 use plane instead of line. This
    // line is better for low DP due to antialiasing, but doesn't
    // support larger line width on macOS.
    if (mDP > 1.0) {
        float angle = atan2((mPoint2.y - mPoint1.y), (mPoint2.x - mPoint1.x));
        float lineWidth = 1.0;
        float xOffset = sin(angle) * lineWidth;
        float yOffset = cos(angle) * lineWidth;

        vertices[0] = mPoint1.x + xOffset;
        vertices[1] = mPoint1.y - yOffset;

        vertices[4] = mPoint2.x + xOffset;
        vertices[5] = mPoint2.y - yOffset;

        vertices[8] =  mPoint1.x - xOffset;
        vertices[9] =  mPoint1.y + yOffset;

        vertices[12] = mPoint2.x - xOffset;
        vertices[13] = mPoint2.y + yOffset;
    } else {
        vertices[0] = mPoint1.x;
        vertices[1] = mPoint1.y;

        vertices[4] = mPoint2.x;
        vertices[5] = mPoint2.y;

        vertices[8] =  mPoint1.x;
        vertices[9] =  mPoint1.y;

        vertices[12] = mPoint2.x;
        vertices[13] = mPoint2.y;
    }

    invalidate();
}