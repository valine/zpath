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

    mHandle = new ZView(vec2(40), highlight, mFirstView);
    mHandle->setGravity(topRight);
    mHandle->setElevation(0.8);
}


bool ZTileViewController::onMouseEvent(int button, int action, int mods, int x, int y) {
    ZViewController::onMouseEvent(button, action, mods, x, y);

    if (action == GLFW_PRESS && mHandle != nullptr && isMouseInBounds(mHandle)) {
        mDragType = cornerDrag;
    } else if(action == GLFW_PRESS) {

        if (mSecondView != nullptr) {

                if (mSplitType == sideBySide && (abs(mFirstView->getRight() - x) < 10)) {
                    mInitialFirst = mFirstView->getWidth();
                    mInitialSecond = mSecondView->getWidth();
                    mDragType = tileDrag;
                }
        }
//
//        else if (mSplitType == overUnder && mSecondView != nullptr) {
//            if (abs(mFirstView->getBottom() - y) < DRAG_THRESHOLD || abs(mSecondView->getTop() - y) < DRAG_THRESHOLD) {
//                mInitialSecond = mFirstView->getHeight();
//                mInitialFirst = mSecondView->getHeight();
//                mDragType = tileDrag;
//
//            }
//        }

    }



    if (action == GLFW_RELEASE) {
        mDragType = noDrag;
    }

    return false;
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

                if (mSecondView != nullptr) {
                    int second = mInitialSecond - (int) delta.x;
                    int first = mInitialFirst + (int) delta.x;

                    mSecondView->setMaxWidth(second);
                    mSecondView->setXOffset(first);
                    mFirstView->setMaxWidth(first);
                    mFirstView->invalidate();
                    mSecondView->invalidate();
                    mSecondView->onWindowChange(getWidth(), getHeight());
                }
            } else if (mSplitType == overUnder) {
                int first = mInitialSecond + (int) delta.y;
                int second = mInitialFirst - (int) delta.y;

                mSecondView->setMaxHeight(second);
                mSecondView->setYOffset(first);
                mFirstView->setMaxHeight(first);
                mFirstView->invalidate();
                mSecondView->invalidate();
                mSecondView->onWindowChange(getWidth(), getHeight());
            }
        }
    } else if (state == mouseUp) {
        mDragType = noDrag;
    }
}

void ZTileViewController::triggerSideSplit() {// Trigger split
    mSecondView = new ZTileViewController(getResourcePath(), mControllerFactory, mNames);
    mSecondView->setParentTile(this);
    mSecondView->setBackgroundColor(getBackgroundColor() - vec4(0.1, 0.1, 0.1, 0.0));
    addSubView(mSecondView);

    mSecondView->setMaxHeight(mFirstView->getMaxHeight());
    mSecondView->setMaxWidth(0);
    mSecondView->setXOffset(mFirstView->getWidth());
    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());

    mInitialFirst = mFirstView->getWidth();
    mInitialSecond = mSecondView->getWidth();

    mSplitType = sideBySide;
    mDragType = tileDrag;
}


void ZTileViewController::triggerOverUnderSplit() {// Trigger split


    mSecondView = new ZTileViewController(getResourcePath(), mControllerFactory, mNames);
    mSecondView->setParentTile(this);
    mSecondView->setBackgroundColor(getBackgroundColor() - vec4(0.1, 0.1, 0.1, 0.0));
    addSubView(mSecondView);

    mSecondView->setMaxWidth(mFirstView->getMaxWidth());
    mSecondView->setMaxHeight(0);
    mSecondView->setYOffset(mFirstView->getHeight());
    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());

    mInitialFirst = mFirstView->getHeight();
    mInitialSecond = mSecondView->getHeight();
    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());

    mSplitType = overUnder;
    mDragType = tileDrag;
}

void ZTileViewController::onGlobalMouseUp(int key) {
    ZViewController::onGlobalMouseUp(key);
    mDragType = noDrag;
}

void ZTileViewController::setParentTile(ZTileViewController* tileView) {
    mParentTile = tileView;
}


