//
// Created by Lukas Valine on 5/19/19.
//

#include <mesh/zobject.h>
#include <utils/zimageutil.h>
#include "ui/zchart.h"

ZChart::ZChart(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight) {
    mRenderer = new ZChartRenderer(maxWidth, maxHeight);
    mRenderer->onDraw();

    invalidate();
    mBackground = new ZTexture(mRenderer->getTexID());

    setBackgroundImage(mBackground);
    parent->addSubView(this);
}

void ZChart::draw() {
    ZView::draw();
    mRenderer->onDraw();
}

void ZChart::addLine(float *points, int size) {
    mRenderer->addLine(points, size);
    mRenderer->onDraw();

}

void ZChart::updateLine(int index, float *points, int size) {
    mRenderer->updateLine(index, points, size);
    mRenderer->onDraw();
//
//    ZUtil::saveGlFBO("/home/lukas/Desktop/test.png", mRenderer->getFrameID(), getWidth(),
                     //     getHeight());
}

void ZChart::onWindowChange(int width, int height) {
    ZView::onWindowChange(width, height);
    mRenderer->setSize(getWidth(), getHeight());
}

void ZChart::setMinMax(float min, float max) {
    mRenderer->setMax(max);
    mRenderer->setMax(min);
}
