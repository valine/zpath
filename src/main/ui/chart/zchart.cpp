//
// Created by Lukas Valine on 5/19/19.
//

#include <mesh/zobject.h>
#include <utils/zutil.h>
#include "ui/zchart.h"

ZChart::ZChart(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight, parent) {
    mRenderer = new ZChartRenderer(maxWidth, maxHeight);
    //mRenderer->onDraw();

    invalidate();
    mBackground = new ZTexture(mRenderer->getTexID());

    setBackgroundImage(mBackground);
}

void ZChart::draw() {
    int index = 0;
    for(bool invalid : mLinesInvalid) {

        if (invalid) {
            if (mRenderer->getLineCount() < mLinesInvalid.size()) {
                mRenderer->addLine(mPendingLines.at(index));
            } else {
                mRenderer->updateLine(index, mPendingLines.at(index));
            }

            invalidate();
            mLinesInvalid.at(index) = false;
        }

        index++;
    }
    mRenderer->onDraw();

    ZView::draw();
}

void ZChart::addLine(float *points, int size) {
    mRenderer->addLine(points, size);
    mRenderer->onDraw();
    invalidate();

}

void ZChart::updateLine(int index, float *points, int size) {
    mRenderer->updateLine(index, points, size);
    mRenderer->onDraw();
    invalidate();

}

void ZChart::onWindowChange(int width, int height) {
    ZView::onWindowChange(width, height);
    mRenderer->setSize(getWidth(), getHeight());
    invalidate();
}

void ZChart::resetZoom() {
    mRenderer->resetZoom(mPendingLines.at(0));
    invalidate();
}

void ZChart::setMinMax(float min, float max) {
    mRenderer->setMin(max);
    mRenderer->setMax(min);
    invalidate();
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
