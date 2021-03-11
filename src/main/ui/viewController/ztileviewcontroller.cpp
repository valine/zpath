#include <utility>

//
// Created by lukas on 3/10/21.
//

#include "ui/ztileviewcontroller.h"

ZTileViewController::ZTileViewController(char **argv, std::function<ZViewController *(int)> factory,
                                         vector<string> names, bool isRoot, ZViewController *content) : ZViewController(argv){
    mControllerFactory = std::move(factory);
    mNames = std::move(names);
    mIsRoot = isRoot;
    mContent = content;
}

ZTileViewController::ZTileViewController(string path, std::function<ZViewController *(int)> factory,
                                         vector<string> names, bool isRoot, ZViewController *content) : ZViewController(std::move(path)) {

    mControllerFactory = std::move(factory);
    mNames = std::move(names);
    mIsRoot = isRoot;
    mContent = content;
}

void ZTileViewController::onLayoutFinished() {
    ZViewController::onLayoutFinished();

    if (mContent == nullptr) {
        mContent = mControllerFactory(1);
    }

    mContent->setDrawingEnabled(false);
    mContent->setOutlineType(outline);
    mContent->setLineWidth(2);

    bool viewAdded = false;
    for (ZView* view : getSubViews()) {
        if (view == mContent) {
            viewAdded = true;
        }
    }

    if (!viewAdded) {
        addSubView(mContent);
        mHandle = new ZView(vec2(40), highlight, this);
        mHandle->setGravity(topRight);
        mHandle->setElevation(0.8);
    }
}

void ZTileViewController::onMouseEvent(int button, int action, int mods, int x, int y) {
  ZViewController::onMouseEvent(button, action, mods, x, y);

    if (action == GLFW_PRESS && mHandle != nullptr && isMouseInBounds(mHandle)) {
        mDragType = cornerDrag;
    } else if(action == GLFW_PRESS) {

        if (mFirstView != nullptr) {
            if (mSplitType == sideBySide && (abs(mFirstView->getRight() - x) < 10)) {
                mInitialFirst = mFirstView->getWidth();
                mInitialSecond = mSecondView->getWidth();
                mDragType = tileDrag;
            }

            if (mSplitType == overUnder && (abs(mFirstView->getBottom() - y) < 10)) {
                mInitialFirst = mSecondView->getHeight();
                mInitialSecond = mFirstView->getHeight();
                mDragType = tileDrag;
            }
        }

    }

    if (action == GLFW_RELEASE) {
        mDragType = noDrag;
    }
}

void ZTileViewController::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) {
    ZView::onMouseDrag(absolute, start, delta, state, button);

    if (state == mouseDrag && isMouseInBounds(this)) {
        if (mDragType == cornerDrag) {

            if (delta.x < -DRAG_THRESHOLD) {
                triggerSideSplit();
            } else if (delta.y > DRAG_THRESHOLD) {
                triggerOverUnderSplit();
            }
        } else if (mDragType == tileDrag) {

            if (mSplitType == sideBySide) {
                int first = mInitialFirst + (int) delta.x;
                mSecondView->setMaxWidth(fillParent);
                mSecondView->setXOffset(first);
                mFirstView->setMaxWidth(first);
                mFirstView->invalidate();
                mSecondView->invalidate();

            } else if (mSplitType == overUnder) {
                int first = mInitialSecond + (int) delta.y;
                mSecondView->setMaxHeight(fillParent);
                mSecondView->setYOffset(first);
                mFirstView->setMaxHeight(first);
                mFirstView->invalidate();
                mSecondView->invalidate();
            }
        }
    } else if (state == mouseUp) {
        mDragType = noDrag;
    }
}

void ZTileViewController::triggerSideSplit() {// Trigger split

    mSplitType = sideBySide;
    mDragType = tileDrag;

    mFirstView = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, mContent);
    mFirstView->setDrawingEnabled(false);
    removeSubView(mContent);
    addSubView(mFirstView);

    mSecondView = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, nullptr);
    mSecondView->setDrawingEnabled(false);
    mSecondView->setBackgroundColor(red);
    addSubView(mSecondView);

    mSecondView->setMaxHeight(mFirstView->getMaxHeight());
    mSecondView->setMaxWidth(0);
    mSecondView->setXOffset(getWidth());

    mInitialFirst = mFirstView->getWidth();
    mInitialSecond = mSecondView->getWidth();

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
    mHandle->setVisibility(false);
}


void ZTileViewController::triggerOverUnderSplit() {// Trigger split

    mSplitType = overUnder;
    mDragType = tileDrag;

    mFirstView = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, mContent);
    mFirstView->setDrawingEnabled(false);
    removeSubView(mContent);
    addSubView(mFirstView);

    mSecondView = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, nullptr);
    mSecondView->setDrawingEnabled(false);
    mSecondView->setBackgroundColor(red);
    addSubView(mSecondView);

    mSecondView->setMaxHeight(0);
    mSecondView->setMaxWidth(mFirstView->getMaxWidth());
    mSecondView->setYOffset(getHeight());

    mInitialFirst = mFirstView->getHeight();
    mInitialSecond = mSecondView->getHeight();

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
    mHandle->setVisibility(false);
}

void ZTileViewController::onGlobalMouseUp(int key) {
    ZViewController::onGlobalMouseUp(key);
    mDragType = noDrag;
}


