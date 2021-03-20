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
    mControllers = vector<ZViewController*>(mNames.size(), nullptr);
}


ZTileViewController::ZTileViewController(string path, std::function<ZViewController *(int)> factory,
                                         vector<string> names, bool isRoot, ZViewController *content) : ZViewController(std::move(path)) {
    mControllerFactory = std::move(factory);
    mNames = std::move(names);
    mIsRoot = isRoot;
    mContent = content;
    mControllers = vector<ZViewController*>(mNames.size(), nullptr);
}

void ZTileViewController::onLayoutFinished() {
    ZViewController::onLayoutFinished();

    int defaultController = 1;

    if (mContent == nullptr) {
        mContent = mControllerFactory(defaultController);
        mControllers.at(defaultController) = mContent;
    }

    mContent->setDrawingEnabled(false);
    mContent->setOutlineType(outline);
    mContent->setLineWidth(2);
    addSubView(mContent);

    mHandle = new ZView(vec2(40), highlight, this);
    mHandle->setGravity(topRight);
    mHandle->setElevation(0.8);

    mDropDown = new ZDropDown(200, 300, mNames, this);
    mDropDown->setGravity(bottomLeft);
    mDropDown->setYOffset(0);
    mDropDown->selectItem(defaultController);

    mDropDown->setOnItemChange([this](int index){
        for (ZViewController* controller : mControllers) {
            if (controller != nullptr) {
                controller->setVisibility(false);
            }
        }

        if(mControllers.at(index) == nullptr) {
            mControllers.at(index) = mControllerFactory(index);
            mContent = mControllers.at(index);
            mContent->setDrawingEnabled(false);
            mContent->setOutlineType(outline);
            mContent->setLineWidth(2);
            addSubView(mContent);
            getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
        } else {
            mControllers.at(index)->setVisibility(true);
            mContent = mControllers.at(index);
        }

        mHandle->bringToFront();
        mDropDown->bringToFront();

        invalidate();

    });

    mIndexLabel = new ZLabel("-1", this);
    mIndexLabel->setBackgroundColor(red);
    mIndexLabel->setGravity(Gravity::bottomLeft);
    mIndexLabel->setYOffset(200);
    mIndexLabel->setMaxWidth(200);
}

void ZTileViewController::onMouseEvent(int button, int action, int mods, int x, int y) {
    ZViewController::onMouseEvent(button, action, mods, x, y);

    if (action == GLFW_PRESS && mHandle != nullptr && mHandle->getVisibility() && isMouseInBounds(mHandle)) {

        ////////////// Step 1: enter corner drag mode
        mDragType = cornerDrag;
    } else if(action == GLFW_PRESS) {
//        if (mLeftTile != nullptr) {
//            if (mSplitType == sideBySide && (abs(mLeftTile->getRight() - x) < 10)) {
//                mInitialPosition = mLeftTile->getWidth();
//                mInitialSecond = mRightTile->getWidth();
//                mDragType = tileDrag;
//            }
//
//            if (mSplitType == overUnder && (abs(mLeftTile->getBottom() - y) < 10)) {
//                mInitialPosition = mRightTile->getHeight();
//                mInitialSecond = mLeftTile->getHeight();
//                mDragType = tileDrag;
//            }
//        }
    }

    if (action == GLFW_RELEASE) {
        mDragType = noDrag;
    }
}

void ZTileViewController::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) {
    ZView::onMouseDrag(absolute, start, delta, state, button);

    if (state == mouseDrag && isMouseInBounds(this)) {
        if (mDragType == cornerDrag) {

            ///////////////Step 2: trigger split or join
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
                auto leftTile = mChildrenTiles.at(mDragIndex - 1);
                auto rightTile = mChildrenTiles.at(mDragIndex);

                int bondary = mInitialBondary + (int) delta.x;

                int rightTileSize = fillParent;
                int leftTileSize = bondary - leftTile->getOffsetX();
                if (mChildrenTiles.size() > mDragIndex + 1) {
                    rightTileSize = mChildrenTiles.at(mDragIndex + 1)->getOffsetX() - bondary;
                }

                rightTile->setXOffset(bondary);
                rightTile->setMaxWidth(rightTileSize);
                leftTile->setMaxWidth(leftTileSize);

                //leftTile->onWindowChange(getWindowWidth(), getWindowHeight());

                rightTile->invalidate();
                leftTile->invalidate();

                rightTile->onWindowChange(rightTile->getWindowWidth(), rightTile->getWindowHeight());

            } else if (mSplitType == overUnder) {
                int first = mInitialSecond + (int) delta.y;
//                mRightTile->setMaxHeight(fillParent);
//                mRightTile->setYOffset(first);
//                mLeftTile->setMaxHeight(first);
//                mLeftTile->invalidate();
//                mRightTile->invalidate();
            }

          //  mRightTile->onWindowChange(getWindowWidth(), getWindowHeight());
          //  mLeftTile->onWindowChange(getWindowWidth(), getWindowHeight());
        }
    } else if (state == mouseUp) {
        releaseFocus(getRootView());
        mDragType = noDrag;
        getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
    }
}

void ZTileViewController::insertChildAtIndex(int index) {
    auto tile = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, nullptr);
    addSubView(tile);

    double offset = mChildrenTiles.at(index - 1)->getOffsetX() +  mChildrenTiles.at(index - 1)->getWidth();
    tile->setDrawingEnabled(false);
    tile->setVisibility(true);
    tile->setMaxWidth(0);
    tile->setXOffset(offset);
    tile->mTileType = horizontalTile;
    tile->mParentTile = this;
   // tile->mHandle->bringToFront();
    tile->mSplitType = sideBySide;
    tile->mDragType = tileDrag;
    mChildrenTiles.insert(mChildrenTiles.begin() + index, tile);


    updateIndices();

    mSplitType = sideBySide;
    mDragType = tileDrag;
    mDragIndex = index;

    mInitialBondary = mChildrenTiles.at(index - 1)->getOffsetX() + mChildrenTiles.at(index - 1)->getWidth();
}

void ZTileViewController::updateIndices() {
    int index = 0;
    for (auto tile : mChildrenTiles) {
        tile->setTileIndex(index);
        index++;
    }
}

void ZTileViewController::triggerSideSplit() {

    /////////////// Step 3: create views after split

    // Check if this needs to be a root tile
    if (mTileType != horizontalTile) {
        removeSubView(mContent);

        ZTileViewController* leftTile;
        ZTileViewController* rightTile;
        if (mChildrenTiles.empty()) {
            leftTile = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, mContent);
            rightTile = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, nullptr);

            addSubView(leftTile);
            addSubView(rightTile);
        } else {
            leftTile = mChildrenTiles.at(0);
            rightTile = mChildrenTiles.at(1);
        }

        leftTile->setParentView(this);
        leftTile->setDrawingEnabled(false);
       // leftTile->mHandle->bringToFront();
        leftTile->setVisibility(true);
        leftTile->setTileIndex(0);
        leftTile->mTileType = horizontalTile;
        leftTile->mParentTile = this;
        mChildrenTiles.push_back(leftTile);


        rightTile->setDrawingEnabled(false);
        rightTile->setVisibility(true);
        rightTile->setMaxWidth(0);
        rightTile->setXOffset(getWidth());
        rightTile->setParentView(this);
        rightTile->mParentTile = this;
        rightTile->mHandle->setBackgroundColor(red);
        rightTile->setTileIndex(1);
       // rightTile->mHandle->bringToFront();
        rightTile->mTileType = horizontalTile;

        mChildrenTiles.push_back(rightTile);

        mSplitType = sideBySide;
        mDragType = tileDrag;
        mDragIndex = 1;

        mInitialBondary = getWidth();

        mHandle->setVisibility(false);
        mDropDown->setVisibility(false);

    } else {
        mParentTile->insertChildAtIndex(mTileIndex + 1);
        mDragType = noDrag;
    }

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());

}



/// Todo: fix later

void ZTileViewController::triggerOverUnderSplit() {// Trigger split

//    mSplitType = overUnder;
//    mDragType = tileDrag;
//
//    mLeftTile = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, mContent);
//    mLeftTile->setDrawingEnabled(false);
//    removeSubView(mContent);
//    addSubView(mLeftTile);
//    mLeftTile->setTileIndex(0);
//
//    mRightTile = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, nullptr);
//    mRightTile->setDrawingEnabled(false);
//    mRightTile->setBackgroundColor(red);
//    addSubView(mRightTile);
//
//    mRightTile->setMaxHeight(0);
//    mRightTile->setMaxWidth(mLeftTile->getMaxWidth());
//    mRightTile->setYOffset(getHeight());
//    mRightTile->setTileIndex(1);
//
//    mInitialPosition = mLeftTile->getHeight();
//    mInitialSecond = mRightTile->getHeight();
//
//    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
//    mHandle->setVisibility(false);
//    mDropDown->setVisibility(false);
}

void ZTileViewController::setTileIndex(int index) {
   // mIndexLabel->setText(to_string(index));
//    mIndexLabel->bringToFront();

    mTileIndex = index;
    switch (index) {
        case 0:
            mIndexLabel->setBackgroundColor(red);
            break;
        case 1:
            mIndexLabel->setBackgroundColor(blue);
            break;
        case 3:
            mIndexLabel->setBackgroundColor(green);
            break;
        case 4:
            mIndexLabel->setBackgroundColor(yellow);
            break;
        case 5:
            mIndexLabel->setBackgroundColor(gold);
            break;
        default:
            mIndexLabel->setBackgroundColor(white);
            break;
    }

}

void ZTileViewController::onGlobalMouseUp(int key) {
    ZViewController::onGlobalMouseUp(key);
    releaseFocus(this);
    mDragType = noDrag;
}

void ZTileViewController::triggerSideJoin() {
//
//    if (mParentTile != nullptr) {
//        if (mParentTile->mLeftTile == this) {
//            ZTileViewController* leftMost = mParentTile->mRightTile->getLeftMostChild();
//            if (leftMost == mParentTile->mRightTile) {
//                mParentTile->triggerSideJoinLeftToRight();
//            } else {
//                leftMost->mParentTile->triggerSideJoinRightToLeft();
//            }
//        } else {
//            if (mParentTile->mParentTile != nullptr) {
//                mParentTile->mParentTile->triggerSideJoinRightToLeft();
//
//            }
//        }
//    }
}

ZTileViewController* ZTileViewController::getLeftMostChild() {
//    if (hasChildren()) {
//        return mLeftTile->getLeftMostChild();
//    }
//
//    return this;
}

void ZTileViewController::triggerSideJoinLeftToRight() {
//    if (mRightTile != nullptr) {
//
//        mLeftTile->removeSubView(mContent);
//        mRightTile->setVisibility(false);
//        mLeftTile->setVisibility(false);
//        addSubView(mContent);
//        mHandle->setVisibility(true);
//        mHandle->bringToFront();
//
//        mInitialPosition = getWidth();
//        mInitialSecond = 0;
//        getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
//    }
}

void ZTileViewController::triggerSideJoinRightToLeft() {

//    // To fix this there needs to be more than left and right tiles
//    if (mRightTile != nullptr) {
//
//        ZViewController* secondContent = mRightTile->mContent;
//        mRightTile->removeSubView(secondContent);
//        mRightTile->setVisibility(false);
//        mLeftTile->setVisibility(false);
//        addSubView(secondContent);
//        mHandle->setVisibility(true);
//        mHandle->bringToFront();
//
//        mInitialPosition = getWidth();
//        mInitialSecond = 0;
//        getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
//    } else {
//        setVisibility(false);
//    }
}

bool ZTileViewController::hasChildren() {
    //return !(mRightTile == nullptr || !mRightTile->getVisibility());
}

