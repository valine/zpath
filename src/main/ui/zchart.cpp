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

    int index = 0;
    for(bool invalid : mLinesInvalid) {

        if (invalid) {
            if (mRenderer->getLineCount() < mLinesInvalid.size()) {
                mRenderer->addLine(mPendingLines.at(index));
            } else {
                mRenderer->updateLine(index, mPendingLines.at(index));
            }
        }

        index++;
    }
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

void ZChart::resetZoom() {
    mRenderer->resetZoom(mPendingLines.at(0));
}

void ZChart::setMinMax(float min, float max) {
    mRenderer->setMin(max);
    mRenderer->setMax(min);
}

void ZChart::addLine(vector<float> line) {
    mPendingLines.push_back(line);
    mLinesInvalid.push_back(true);
    invalidate();
}

void ZChart::updateLine(int index, vector<float> line) {
    mPendingLines.at(index) = line;
    mLinesInvalid.at(index) = true;
    invalidate();
}
