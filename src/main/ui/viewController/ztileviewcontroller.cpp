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

    ZView::onWindowChange(getWindowWidth(), getWindowHeight());

}

void ZTileViewController::onMouseEvent(int button, int action, int mods, int x, int y) {
    ZViewController::onMouseEvent(button, action, mods, x, y);

    if (action == GLFW_PRESS && mHandle != nullptr && isMouseInBounds(mHandle)) {
        mDragType = cornerDrag;
    } else if(action == GLFW_PRESS) {
        if (mLeftTile != nullptr) {
            if (mSplitType == sideBySide && (abs(mLeftTile->getRight() - x) < 10)) {
                mInitialFirst = mLeftTile->getWidth();
                mInitialSecond = mRightTile->getWidth();
                mDragType = tileDrag;
            }

            if (mSplitType == overUnder && (abs(mLeftTile->getBottom() - y) < 10)) {
                mInitialFirst = mRightTile->getHeight();
                mInitialSecond = mLeftTile->getHeight();
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
            } else if (delta.x > DRAG_THRESHOLD) {

                if (mParentTile != nullptr) {
                    triggerSideJoin();
                }

                mDragType = noDrag;
            } else if (delta.y > DRAG_THRESHOLD) {
                triggerOverUnderSplit();
            }
        } else if (mDragType == tileDrag) {

            if (mSplitType == sideBySide) {
                int first = mInitialFirst + (int) delta.x;
                mRightTile->setMaxWidth(fillParent);
                mRightTile->setXOffset(first);
                mLeftTile->setMaxWidth(first);
                mLeftTile->invalidate();
                mRightTile->invalidate();

            } else if (mSplitType == overUnder) {
                int first = mInitialSecond + (int) delta.y;
                mRightTile->setMaxHeight(fillParent);
                mRightTile->setYOffset(first);
                mLeftTile->setMaxHeight(first);
                mLeftTile->invalidate();
                mRightTile->invalidate();
            }

            mRightTile->onWindowChange(getWindowWidth(), getWindowHeight());
            mLeftTile->onWindowChange(getWindowWidth(), getWindowHeight());
        }
    } else if (state == mouseUp) {
        releaseFocus(getRootView());
        mDragType = noDrag;
        getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
    }
}
void ZTileViewController::triggerSideJoin() {

    if (mParentTile != nullptr) {
        if (mParentTile->mLeftTile == this) {
            ZTileViewController* leftMost = mParentTile->mRightTile->getLeftMostChild();
            if (leftMost == mParentTile->mRightTile) {
                mParentTile->triggerSideJoinLeftToRight();
            } else {
                leftMost->mParentTile->triggerSideJoinRightToLeft();
            }
        } else {
            if (mParentTile->mParentTile != nullptr) {
                mParentTile->mParentTile->triggerSideJoinRightToLeft();

            }
        }
    }
}

ZTileViewController* ZTileViewController::getLeftMostChild() {
    if (hasChildren()) {
        return mLeftTile->getLeftMostChild();
    }

    return this;
}

void ZTileViewController::triggerSideJoinLeftToRight() {
    if (mRightTile != nullptr) {

        mLeftTile->removeSubView(mContent);
        mRightTile->setVisibility(false);
        mLeftTile->setVisibility(false);
        addSubView(mContent);
        mHandle->setVisibility(true);
        mHandle->bringToFront();

        mInitialFirst = getWidth();
        mInitialSecond = 0;
        getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
    }
}

void ZTileViewController::triggerSideJoinRightToLeft() {

    // To fix this there needs to be more than left and right tiles
    if (mRightTile != nullptr) {

        ZViewController* secondContent = mRightTile->mContent;
        mRightTile->removeSubView(secondContent);
        mRightTile->setVisibility(false);
        mLeftTile->setVisibility(false);
        addSubView(secondContent);
        mHandle->setVisibility(true);
        mHandle->bringToFront();

        mInitialFirst = getWidth();
        mInitialSecond = 0;
        getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
    } else {
        setVisibility(false);
    }
}



bool ZTileViewController::hasChildren() {
    return !(mRightTile == nullptr || !mRightTile->getVisibility());
}

void ZTileViewController::triggerSideSplit() {// Trigger split
    removeSubView(mContent);

    if (mLeftTile == nullptr) {
        mLeftTile = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, mContent);
        mLeftTile->setDrawingEnabled(false);
        addSubView(mLeftTile);
    } else {
        mLeftTile->addSubView(mContent);
        mLeftTile->mHandle->bringToFront();
    }
    mLeftTile->mParentTile = this;
    mLeftTile->setVisibility(true);


    if (mRightTile == nullptr) {
        mRightTile = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, nullptr);
        mRightTile->setDrawingEnabled(false);
        addSubView(mRightTile);
    }

    mRightTile->setVisibility(true);
    mRightTile->mParentTile = this;
    mRightTile->setBackgroundColor(red);


    mRightTile->setMaxHeight(mLeftTile->getMaxHeight());
    mRightTile->setMaxWidth(0);
    mRightTile->setXOffset(getWidth());

    mInitialFirst = getWidth();
    mInitialSecond = 0;

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
    mHandle->setVisibility(false);

    mSplitType = sideBySide;
    mDragType = tileDrag;
}


void ZTileViewController::triggerOverUnderSplit() {// Trigger split

    mSplitType = overUnder;
    mDragType = tileDrag;

    mLeftTile = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, mContent);
    mLeftTile->setDrawingEnabled(false);
    removeSubView(mContent);
    addSubView(mLeftTile);

    mRightTile = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, nullptr);
    mRightTile->setDrawingEnabled(false);
    mRightTile->setBackgroundColor(red);
    addSubView(mRightTile);

    mRightTile->setMaxHeight(0);
    mRightTile->setMaxWidth(mLeftTile->getMaxWidth());
    mRightTile->setYOffset(getHeight());

    mInitialFirst = mLeftTile->getHeight();
    mInitialSecond = mRightTile->getHeight();

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
    mHandle->setVisibility(false);
}

void ZTileViewController::onGlobalMouseUp(int key) {
    ZViewController::onGlobalMouseUp(key);
    releaseFocus(this);
    mDragType = noDrag;
}


