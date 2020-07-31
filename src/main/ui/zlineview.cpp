//
// Created by lukas on 7/27/20.
//

#include "ui/zlineview.h"




ZLineView::ZLineView(vec2 point1, vec2 point2, ZView *parent): ZView(fillParent, fillParent, parent)  {
    mPoint1 = point1;
    mPoint2 = point2;

    init();
}

void ZLineView::init() {
    setBackgroundColor(vec4(1,0,0,1));
    setDrawWire(true);
}

void ZLineView::computeBounds() {
    ZView::computeBounds();
    calculateLeft();
    calculateTop();
    calculateBottom();
    calculateRight();

    float* vertices = getVertices();

    vertices[0] = getLeft() + mPoint1.x;
    vertices[1] = getTop() + mPoint1.y;

    vertices[4] = getLeft() + mPoint2.x;
    vertices[5] = getTop() + mPoint2.y;

    vertices[8] =  vertices[0];
    vertices[9] =  vertices[1];

    vertices[12] = vertices[4];
    vertices[13] =  vertices[5];

    glBindBuffer(GL_ARRAY_BUFFER, getVertexBuffer());
    glBufferSubData(GL_ARRAY_BUFFER, 0, 16 * sizeof(float), vertices);

    invalidate();
}