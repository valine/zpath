#include <utility>

//
// Created by lukas on 3/10/21.
//

#include "ui/ztileviewcontroller.h"

ZTileViewController::ZTileViewController(char **argv, std::function<ZViewController *(int)> factory,
                                         vector<string> names) : ZViewController(argv){
    mControllerFactory = std::move(factory);
    mNames = std::move(names);
}

ZTileViewController::ZTileViewController(string path, std::function<ZViewController *(int)> factory,
                                         vector<string> names) : ZViewController(std::move(path)) {

    mControllerFactory = std::move(factory);
    mNames = std::move(names);
}

void ZTileViewController::onLayoutFinished() {
    ZViewController::onLayoutFinished();

   // setBackgroundColor(ZSettingsStore::getInstance().getBackgroundColor());

    mFirstView = mControllerFactory(1);
    mFirstView->setDrawingEnabled(false);
    mFirstView->setOutlineColor(black);
    mFirstView->setOutlineType(WireType::outline);
    addSubView(mFirstView);

    mHandle = new ZView(vec2(40), red, mFirstView);
    mHandle->setGravity(topRight);
    mHandle->setElevation(0.8);
}


bool ZTileViewController::onMouseEvent(int button, int action, int mods, int x, int y) {
    ZViewController::onMouseEvent(button, action, mods, x, y);

    if (action == GLFW_PRESS && mHandle != nullptr && isMouseInBounds(mHandle)) {
        mDragType = cornerDrag;
    } if (action == GLFW_RELEASE) {
        mDragType = noDrag;
    }
}

void ZTileViewController::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) {
    ZView::onMouseDrag(absolute, start, delta, state, button);

    if (mDragType == cornerDrag) {

        if (delta.x < -5) {
            triggerSideSplit();
        } else if (delta.y > 5) {
            triggerOverUnderSplit();
        }
    }
}

void ZTileViewController::triggerSideSplit() {// Trigger split
    int newWidth = mFirstView->getWidth() / 2;
    mFirstView->setMaxWidth(newWidth);
    mFirstView->invalidate();

    mSecondView = new ZTileViewController(getResourcePath(), mControllerFactory, mNames);
    addSubView(mSecondView);
    mSecondView->setMaxWidth(newWidth);
    mSecondView->setMaxHeight(mFirstView->getMaxHeight());
    mSecondView->setXOffset(newWidth);
    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());

    mSplitType = sideBySide;
    mDragType = tileDrag;
}


void ZTileViewController::triggerOverUnderSplit() {// Trigger split
    int newHeight = mFirstView->getHeight() / 2;
    mFirstView->setMaxHeight(newHeight);
    mFirstView->invalidate();

    mSecondView = new ZTileViewController(getResourcePath(), mControllerFactory, mNames);
    addSubView(mSecondView);
    mSecondView->setMaxHeight(newHeight);
    mSecondView->setMaxWidth(mFirstView->getMaxWidth());
    mSecondView->setYOffset(newHeight);

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());

    mSplitType = overUnder;
    mDragType = tileDrag;
}


