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
    mContent->setYOffset(1);
    mContent->setXOffset(1);
    addSubView(mContent);

    mHandle = new ZView(vec2(30), this);
    mHandle->setGravity(topRight);
    mHandle->setBackgroundImage(new ZTexture(getResourcePath() + "resources/icons/tile.png"));

    mDropDown = new ZDropDown(200, 300, mNames, this);
    mDropDown->setGravity(bottomLeft);
    mDropDown->setYOffset(0);
    mDropDown->selectItem(defaultController);

    mJoinGuide = new ZView(fillParent,fillParent, this);
    mJoinGuide->setBackgroundColor(vec4(0.1,0.1,0.1,0.4));
    mJoinGuide->setVisibility(false);

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
    mIndexLabel->setGravity(Gravity::bottomRight);
    mIndexLabel->setYOffset(10);
    mIndexLabel->setMaxWidth(30);
}

void ZTileViewController::onMouseEvent(int button, int action, int mods, int x, int y) {
    ZViewController::onMouseEvent(button, action, mods, x, y);

    if (action == GLFW_PRESS && mHandle != nullptr && mHandle->getVisibility() && isMouseInBounds(mHandle)) {

        ////////////// Step 1: enter corner drag mode
        mDragType = cornerDrag;
    } else if(action == GLFW_PRESS) {
        int index = 0;
        for (auto tile : mChildrenTiles) {
            if (mSplitType == sideBySide && (abs(tile->getRight() - x) < 10) && !isMouseInBounds(tile->mHandle)) {
                if (mChildrenTiles.size() > tile->mTileIndex + 1) {
                    mInitialBondary = mChildrenTiles.at(tile->mTileIndex + 1)->getOffsetX();
                    mDragType = tileDrag;
                    mDragIndex = tile->mTileIndex + 1;
                    break;
                }
            }

            if (mSplitType == overUnder && (abs(tile->getBottom() - y) < 10) &&
                !isMouseInBounds(mChildrenTiles.at(tile->mTileIndex - 1))) {
                if (tile->mTileIndex > 0) {
                    mInitialBondary = mChildrenTiles.at(tile->mTileIndex - 1)->getOffsetY();
                    mDragType = tileDrag;
                    mDragIndex = tile->mTileIndex;
                    break;
                }
            }

            index++;
        }
    }

    if (action == GLFW_RELEASE) {
        if (mDragType == pendingHorizontalJoin) {
            mJoinGuide->setVisibility(false);
            if (mParentTile != nullptr) {
                mParentTile->triggerSideJoinLeftToRight(mTileIndex);
            }
        } else if (mDragType == pendingVerticalJoin) {
            mJoinGuide->setVisibility(false);
            if (mParentTile != nullptr) {
                mParentTile->triggerJoinBottomToTop(mTileIndex);
            }
        }
        mDragType = noDrag;

    }
}

void ZTileViewController::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) {
    ZView::onMouseDrag(absolute, start, delta, state, button);


        if (state == mouseDrag ) {
            if (mDragType == cornerDrag && isMouseInBounds(this)) {

                ///////////////Step 2: trigger split or join
                if (delta.x < -DRAG_THRESHOLD) {
                    triggerSideSplit();
                } else if (delta.x > DRAG_THRESHOLD) {
                    mDragType = pendingHorizontalJoin;
                    mJoinGuide->setVisibility(true);
                    mJoinGuide->bringToFront();
                } else if (delta.y > DRAG_THRESHOLD) {
                    triggerOverUnderSplit();
                } else if (delta.y < -DRAG_THRESHOLD) {
                    mDragType = pendingVerticalJoin;
                    mJoinGuide->setVisibility(true);
                    mJoinGuide->bringToFront();
                }
            } else if (mDragType == tileDrag && isMouseInBounds(this)) {

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

                    leftTile->onWindowChange(getWindowWidth(), getWindowHeight());

                    rightTile->invalidate();
                    leftTile->invalidate();

                    rightTile->onWindowChange(rightTile->getWindowWidth(), rightTile->getWindowHeight());

                } else if (mSplitType == overUnder) {
                    auto bottomTile = mChildrenTiles.at(mDragIndex - 1);
                    auto topTile = mChildrenTiles.at(mDragIndex);

                    int bondary = mInitialBondary + (int) delta.y;

                    int bottomTileSize = fillParent;
                    int topTileSize = bondary - (topTile->getOffsetY());
                    if (mDragIndex - 2 >= 0) {
                        bottomTileSize = mChildrenTiles.at(mDragIndex - 2)->getOffsetY() - bondary;
                    }

                    bottomTile->setYOffset(bondary);
                    bottomTile->setMaxHeight(bottomTileSize);
                    topTile->setMaxHeight(topTileSize);

                    topTile->onWindowChange(getWindowWidth(), getWindowHeight());

                    bottomTile->invalidate();
                    topTile->invalidate();

                    getRootView()->onWindowChange(bottomTile->getWindowWidth(), bottomTile->getWindowHeight());
                }
            }

            else if (mDragType == pendingHorizontalJoin) {
                if (isMouseInBounds(this)) {
                    mJoinGuide->setVisibility(true);
                    if (mParentTile->mChildrenTiles.size() > mTileIndex + 1) {
                        mParentTile->mChildrenTiles.at(mTileIndex + 1)->mJoinGuide->setVisibility(false);
                    }
                } else if (isMouseInBounds(mParentTile->mChildrenTiles.at(mTileIndex + 1))) {
                    if (mParentTile->mChildrenTiles.size() > mTileIndex + 1) {
                        mParentTile->mChildrenTiles.at(mTileIndex + 1)->mJoinGuide->setVisibility(true);
                    }
                    mJoinGuide->setVisibility(false);
                }

            } else if (mDragType == pendingVerticalJoin) {
                    if (isMouseInBounds(this)) {
                        mJoinGuide->setVisibility(true);
                        //  if (0 > mTileIndex - 1) {
                        mParentTile->mChildrenTiles.at(mTileIndex + 1)->mJoinGuide->setVisibility(false);
                        // }
                    } else if (isMouseInBounds(mParentTile->mChildrenTiles.at(mTileIndex + 1))) {
                        //  if (0 > mTileIndex - 1) {
                        mParentTile->mChildrenTiles.at(mTileIndex + 1)->mJoinGuide->setVisibility(true);
                        //  }
                        mJoinGuide->setVisibility(false);
                    }

            }
        } else if (state == mouseUp) {
            releaseFocus(getRootView());
            mDragType = noDrag;
            getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
        }


}

void ZTileViewController::insertChildAtIndexHorizontal(int index) {
    auto tile = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, nullptr);
    addSubView(tile);

    double offset = mChildrenTiles.at(index - 1)->getOffsetX() +  mChildrenTiles.at(index - 1)->getWidth();
    tile->setDrawingEnabled(false);
    tile->setVisibility(true);
    tile->setMaxWidth(0);
    tile->setXOffset(offset);
    tile->mTileType = horizontalTile;
    tile->mParentTile = this;
    tile->mSplitType = sideBySide;
    tile->mDragType = tileDrag;
    mChildrenTiles.insert(mChildrenTiles.begin() + index, tile);

    updateIndices();

    mSplitType = sideBySide;
    mDragType = tileDrag;
    mDragIndex = index;

    mInitialBondary = mChildrenTiles.at(index - 1)->getOffsetX() + mChildrenTiles.at(index - 1)->getWidth();
}

void ZTileViewController::insertChildAtIndexVertical(int index) {
    auto tile = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, nullptr);
    addSubView(tile);

    double offset = mChildrenTiles.at(index - 1)->getOffsetY();
    tile->setDrawingEnabled(false);
    tile->setVisibility(true);
    tile->setMaxHeight(0);
    tile->setYOffset(offset);
    tile->mTileType = verticalTile;
    tile->mParentTile = this;
    tile->mSplitType = overUnder;
    tile->mDragType = tileDrag;
    mChildrenTiles.insert(mChildrenTiles.begin() + index, tile);

    updateIndices();

    mSplitType = overUnder;
    mDragType = tileDrag;
    mDragIndex = index;

    mInitialBondary = mChildrenTiles.at(index - 1)->getOffsetY();
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
        mParentTile->insertChildAtIndexHorizontal(mTileIndex + 1);
        mDragType = noDrag;
    }

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());

}

/// Todo: fix later

void ZTileViewController::triggerOverUnderSplit() {// Trigger split

    /////////////// Step 3: create views after split

    // Check if this needs to be a root tile
    if (mTileType != verticalTile) {
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
        leftTile->mTileType = verticalTile;
        leftTile->mParentTile = this;
        mChildrenTiles.push_back(leftTile);


        rightTile->setDrawingEnabled(false);
        rightTile->setVisibility(true);
        rightTile->setMaxHeight(0);
        rightTile->setYOffset(0);
        rightTile->setParentView(this);
        rightTile->mParentTile = this;
        rightTile->setTileIndex(1);
        // rightTile->mHandle->bringToFront();
        rightTile->mTileType = verticalTile;

        mChildrenTiles.push_back(rightTile);

        mSplitType = overUnder;
        mDragType = tileDrag;
        mDragIndex = 1;

        mInitialBondary = 0;

        mHandle->setVisibility(false);
        mDropDown->setVisibility(false);

    } else {
        mParentTile->insertChildAtIndexVertical(mTileIndex + 1);
        mDragType = noDrag;
    }

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
}

void ZTileViewController::setTileIndex(int index) {
    mIndexLabel->setText(to_string(index));
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
    mJoinGuide->setVisibility(false);
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

void ZTileViewController::triggerSideJoinLeftToRight(int index) {

    mDragType = noDrag;
    if (mChildrenTiles.size() > index + 1) {
        removeSubView(mChildrenTiles.at(index + 1));
        mChildrenTiles.erase(mChildrenTiles.begin() + (index + 1));
        updateIndices();
        int rightSide = mChildrenTiles.at(index)->getOffsetX();
        int width = getWidth();
        if (mChildrenTiles.size() > index + 1) {
            width = mChildrenTiles.at(index + 1)->getOffsetX() - rightSide;
        }
        mChildrenTiles.at(index)->setMaxWidth(width);
    }

    resetController();
    updateIndices();
    onWindowChange(getWindowWidth(),getWindowHeight());
    mDragType = noDrag;

}

void ZTileViewController::resetController() {
    if (mChildrenTiles.size() == 1) {
        ZViewController* content = mChildrenTiles.at(0)->mContent;
        mChildrenTiles.at(0)->removeSubView(content);
        removeSubView(mChildrenTiles.at(0));
        mChildrenTiles.erase(mChildrenTiles.begin() + 0);
        addSubView(content);
        mHandle->setVisibility(true);
        mDropDown->setVisibility(true);
        mHandle->bringToFront();
        mDropDown->bringToFront();
    }
}

void ZTileViewController::triggerJoinBottomToTop(int index) {

    mDragType = noDrag;
    if (mChildrenTiles.size() > index + 1) {
        removeSubView(mChildrenTiles.at(index + 1));
        mChildrenTiles.erase(mChildrenTiles.begin() + (index + 1));
        updateIndices();

        int offset = 0;
        int height = getHeight();
        if (mChildrenTiles.size() > index + 1) {
            offset = mChildrenTiles.at(index + 1)->getOffsetY() + mChildrenTiles.at(index + 1)->getMaxHeight();
        }

        if (index - 1 >= 0) {
            height = mChildrenTiles.at(index - 1)->getOffsetY();
        }
        mChildrenTiles.at(index)->setMaxHeight(height);
        mChildrenTiles.at(index)->setYOffset(offset);
    }

    resetController();
    updateIndices();

    onWindowChange(getWindowWidth(),getWindowHeight());
    mDragType = noDrag;
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

