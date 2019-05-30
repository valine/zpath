//
// Created by Lukas Valine on 5/19/19.
//

#include <mesh/zobject.h>
#include <utils/zimageutil.h>
#include "ui/zchart.h"

ZChart::ZChart(float maxWidth, float maxHeight, string resources) : ZView(maxWidth, maxHeight) {
    mRenderer = new ZChartRenderer(maxWidth, maxHeight);
    mRenderer->onDraw();

    mBackground = new ZTexture(mRenderer->getTexID());


    ZImageUtil::saveGlImage("/home/lukas/Desktop/test.png", mRenderer->getTexID(), maxWidth, maxHeight);


    setBackgroundImage(mBackground);
}

void ZChart::draw() {
    ZView::draw();
    mRenderer->onDraw();
}

void ZChart::addLine(float *points, int size) {
    mRenderer->addLine(points, size);
    mRenderer->onDraw();
}

void ZChart::updateLine(int index, float *points) {
    mRenderer->updateLine(index, points);
}

void ZChart::onWindowChange(int width, int height) {
    ZView::onWindowChange(width, height);
    mRenderer->setSize(width, height);
}
