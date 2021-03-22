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

    setBackgroundColor(darkerGrey);
    int defaultController = 1;

    if (mParentTile != nullptr) {
        defaultController = mParentTile->getIndexTag();
    }

    if (mContent == nullptr) {
        mContent = mControllerFactory(defaultController);
        mControllers.at(defaultController) = mContent;
    } else if (mParentTile != nullptr) {
        defaultController = mParentTile->getIndexTag();
    }

    setIndexTag(defaultController);

    int boarder = 1;
    mContent->setDrawingEnabled(false);
    mContent->setOutlineType(outline);
    mContent->setLineWidth(2);
    mContent->setYOffset(boarder);
    mContent->setXOffset(boarder);
    mContent->setMarginBottom(BOTTOM_MARGIN);
    addSubView(mContent);

    mHandle = new ZView(vec2(30), this);
    mHandle->setGravity(topRight);
    mHandle->setBackgroundImage(new ZTexture(getResourcePath() + "resources/icons/tile.png"));

    mDropDown = new ZDropDown(100, 300, mNames, this);
    mDropDown->setGravity(bottomLeft);
    mDropDown->setYOffset(0);
    mDropDown->setXOffset(boarder);
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
            mContent->setYOffset(1);
            mContent->setXOffset(1);
            mContent->setMarginBottom(BOTTOM_MARGIN);
            addSubView(mContent);
            getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
        } else {
            mControllers.at(index)->setVisibility(true);
            mContent = mControllers.at(index);
        }

        setIndexTag(index);
        mHandle->bringToFront();
        mDropDown->bringToFront();

        invalidate();

    });

    mIndexLabel = new ZLabel("-1", this);
    mIndexLabel->setGravity(Gravity::bottomRight);
    mIndexLabel->setYOffset(2);
    mIndexLabel->setTextColor(grey);
    mIndexLabel->setMaxWidth(15);
}

void ZTileViewController::onMouseEvent(int button, int action, int mods, int x, int y) {
    if (action == GLFW_PRESS && mHandle != nullptr && mHandle->getVisibility() && isMouseInBounds(mHandle)) {

        ////////////// Step 1: enter corner drag mode
        mDragType = cornerDrag;
        setConsumeClicks(true);
    } else if(action == GLFW_PRESS) {
        int index = 0;
        for (auto tile : mChildrenTiles) {
            int threshold = 10;
            if (mSplitType == sideBySide && (abs(tile->getRight() - x) < threshold) && !isMouseInBounds(tile->mHandle)) {
                if (mChildrenTiles.size() > tile->mTileIndex + 1) {
                    mInitialBondary = mChildrenTiles.at(tile->mTileIndex + 1)->getOffsetX();
                    mDragType = tileDrag;
                    mDragIndex = tile->mTileIndex + 1;
                    setConsumeClicks(true);
                    break;
                }
            }

            if (mSplitType == overUnder && (abs(tile->getBottom() - y) < threshold && mChildrenTiles.size() > tile->mTileIndex - 1) &&
                !isMouseInBounds(mChildrenTiles.at(tile->mTileIndex - 1))) {
                if (tile->mTileIndex > 0) {
                    mInitialBondary = mChildrenTiles.at(tile->mTileIndex - 1)->getOffsetY();
                    mDragType = tileDrag;
                    mDragIndex = tile->mTileIndex;
                    setConsumeClicks(true);
                    break;
                }
            }

            index++;
        }
    }

    ZViewController::onMouseEvent(button, action, mods, x, y);

    if (action == GLFW_RELEASE) {
        if (mDragType == pendingHorizontalJoin) {
            mJoinGuide->setVisibility(false);
            if (mParentTile != nullptr) {
                if (mParentTile->mJoinType == keepFirst) {
                    if (mParentTile->mChildrenTiles.size() > 1) {
                        mParentTile->triggerSideJoinLeftToRight(mTileIndex);
                    }
                } else {
                    if (mParentTile->mChildrenTiles.size() > 1) {
                        mParentTile->triggerSideJoinRightToLeft(mTileIndex);
                    }
                }
            }
        } else if (mDragType == pendingVerticalJoin) {
            mJoinGuide->setVisibility(false);
            if (mParentTile != nullptr) {
                if (mParentTile->mJoinType == keepFirst) {
                    if (mParentTile->mChildrenTiles.size() > 1) {
                        mParentTile->triggerJoinBottomToTop(mTileIndex);
                    }
                } else {
                    if (mParentTile->mChildrenTiles.size() > 1) {
                        mParentTile->triggerJoinTopToBottom(mTileIndex);
                    }
                }
            }
        }
        mDragType = noDrag;
        setConsumeClicks(false);

    }


}

void ZTileViewController::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) {
    ZView::onMouseDrag(absolute, start, delta, state, button);
        if (state == mouseDrag ) {
            if (mDragType == cornerDrag) {

                ///////////////Step 2: trigger split or join
                if (delta.x < -DRAG_THRESHOLD) {
                    triggerSideSplit();
                } else if (delta.x > DRAG_THRESHOLD) {
                    mDragType = pendingHorizontalJoin;
                    updateHorizontalJoinGuide();
                    mJoinGuide->bringToFront();

                    if (mParentTile != nullptr && mParentTile->mChildrenTiles.size() > mTileIndex + 1) {
                        mParentTile->mChildrenTiles.at(mTileIndex + 1)->mJoinGuide->bringToFront();
                    }
                } else if (delta.y > DRAG_THRESHOLD) {
                    triggerOverUnderSplit();
                } else if (delta.y < -DRAG_THRESHOLD) {
                    mDragType = pendingVerticalJoin;
                    updateVerticalJoinGuide();
                    mJoinGuide->bringToFront();
                    if (mParentTile != nullptr && mParentTile->mChildrenTiles.size() > mTileIndex + 1) {
                        mParentTile->mChildrenTiles.at(mTileIndex + 1)->mJoinGuide->bringToFront();
                    }
                }
            } else if (mDragType == tileDrag) {
                onTileEdgeDrag(delta);
            } else if (mDragType == pendingHorizontalJoin) {
                updateHorizontalJoinGuide();

            } else if (mDragType == pendingVerticalJoin) {
                updateVerticalJoinGuide();

            }
        } else if (state == mouseUp) {
            setConsumeClicks(false);
            mDragType = noDrag;
            getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
        }
}

void ZTileViewController::onTileEdgeDrag(const vec2 &delta) {

    int margin = 50;

    if (mSplitType == sideBySide) {
        auto leftTile = mChildrenTiles.at(mDragIndex - 1);
        auto rightTile = mChildrenTiles.at(mDragIndex);

        int max = getWidth() - margin;
        if (mChildrenTiles.size() > mDragIndex + 1) {
            max = getWidth() - ((mChildrenTiles.size() - mDragIndex) * margin);
        }

        int min = mDragIndex * margin;
        int boundary = std::max(std::min(mInitialBondary + (int) delta.x, max), min);

        int rightTileSize = fillParent;
        int leftTileSize = boundary - leftTile->getOffsetX();
        if (mChildrenTiles.size() > mDragIndex + 1) {
            rightTileSize = mChildrenTiles.at(mDragIndex + 1)->getOffsetX() - boundary;
        }

        rightTile->setXOffset(boundary);
        rightTile->setMaxWidth(rightTileSize);
        leftTile->setMaxWidth(leftTileSize);

        for (int i = mDragIndex + 1; i < mChildrenTiles.size(); i++) {
            auto prevTile = mChildrenTiles.at(i - 1);
            auto tile = mChildrenTiles.at(i);
            int prevSide = prevTile->getOffsetX();
            if (prevSide + margin > tile->getOffsetX() ) {
                int maxOffset = getWidth() - margin;
                if (mChildrenTiles.size() > i + 1) {
                    maxOffset = getWidth() - ((mChildrenTiles.size() - i) * margin);
                }

                tile->setXOffset(std::min(maxOffset,prevSide + margin));
                tile->onWindowChange(getWindowWidth(), getWindowHeight());
                tile->invalidate();
            }

            int lastWidth = prevTile->getMaxWidth();
            int width = tile->getOffsetX() - prevTile->getOffsetX();

            if (width != lastWidth) {
                prevTile->setMaxWidth(width);
                prevTile->onWindowChange(getWindowWidth(), getWindowHeight());
            }
        }

        for (int i = mDragIndex - 1; i >= 0; i--) {
            auto prevTile = mChildrenTiles.at(i + 1);
            auto tile = mChildrenTiles.at(i);

            int prevSide = prevTile->getOffsetX();

            bool needsWindow = false;
            if (prevSide - margin < tile->getOffsetX()) {
                int maxOffset = ((i) * margin);
                tile->setXOffset(std::max(maxOffset, prevSide - margin));
                needsWindow = true;
            }

            if (prevSide - margin < tile->getOffsetX() + tile->getMaxWidth()) {
                int width = prevSide - tile->getOffsetX();
                tile->setMaxWidth(width);
                needsWindow = true;
            }

            if (needsWindow) {
                tile->onWindowChange(getWindowWidth(), getWindowHeight());
                tile->invalidate();
            }
        }

        rightTile->invalidate();
        leftTile->invalidate();

        leftTile->onWindowChange(getWindowWidth(), getWindowHeight());
        rightTile->onWindowChange(rightTile->getWindowWidth(), rightTile->getWindowHeight());

    } else if (mSplitType == overUnder) {
        auto bottomTile = mChildrenTiles.at(mDragIndex - 1);
        auto topTile = mChildrenTiles.at(mDragIndex);

        int max = (mChildrenTiles.size() - mDragIndex) * margin;
        int min = getHeight() - ((mDragIndex) * margin);
        int boundary = std::min(min, std::max(max, mInitialBondary + (int) delta.y));

        int bottomTileSize = fillParent;
        int topTileSize = boundary - (topTile->getOffsetY());
        if (mDragIndex - 2 >= 0) {
            bottomTileSize = mChildrenTiles.at(mDragIndex - 2)->getOffsetY() - boundary;
        }

        bottomTile->setYOffset(std::max(boundary, max));
        bottomTile->setMaxHeight(std::max(margin, bottomTileSize));
        topTile->setMaxHeight(std::max(margin, topTileSize));

        for (int i = mDragIndex + 1; i < mChildrenTiles.size(); i++) {
            auto upperTile = mChildrenTiles.at(i);
            auto thisTile = mChildrenTiles.at(i - 1);
            auto lowerTile = mChildrenTiles.at(i - 2);

            int lowerTop = lowerTile->getLocalTop();
            int max = (mChildrenTiles.size() - (i)) * margin;
            if (lowerTop - margin < thisTile->getLocalTop()) {
                upperTile->setMaxHeight( std::max(margin, (lowerTop -upperTile->getLocalTop()) - margin));
                thisTile->setYOffset(std::max(max, lowerTop - margin));
            }

            lowerTile->onWindowChange(getWindowWidth(), getWindowHeight());
            lowerTile->invalidate();

            thisTile->onWindowChange(getWindowWidth(), getWindowHeight());
            thisTile->invalidate();

            upperTile->onWindowChange(getWindowWidth(), getWindowHeight());
            upperTile->invalidate();
        }

        for (int i = mDragIndex - 1; i >= 0; i--) {

            auto lowerTile = mChildrenTiles.at(i);
            auto thisTile = mChildrenTiles.at(i + 1);


            int height = fillParent;
            if (i - 1 >= 0) {
                auto bottomTile = mChildrenTiles.at(i - 1);
                height = abs(lowerTile->getLocalTop() - bottomTile->getLocalTop());
            }

            if (thisTile->getLocalTop() + margin > lowerTile->getLocalTop()) {
                lowerTile->setYOffset(thisTile->getLocalTop() + margin);
                lowerTile->setMaxHeight(height);
            }

            lowerTile->onWindowChange(getWindowWidth(), getWindowHeight());
            lowerTile->invalidate();


        }

        topTile->onWindowChange(getWindowWidth(), getWindowHeight());

        bottomTile->invalidate();
        topTile->invalidate();

        getRootView()->onWindowChange(bottomTile->getWindowWidth(), bottomTile->getWindowHeight());
    }
}

void ZTileViewController::updateVerticalJoinGuide() {
    if (mParentTile != nullptr && (mParentTile->mChildrenTiles.size() > mTileIndex + 1) && isMouseInBounds(this) &&
        mParentTile->mChildrenTiles.at(mTileIndex + 1)->mJoinGuide->getVisibility()) {
        mJoinGuide->setVisibility(true);
        mParentTile->mChildrenTiles.at(mTileIndex + 1)->mJoinGuide->setVisibility(false);
        mParentTile->mJoinType = keepFirst;
    }

    if (mParentTile != nullptr && (mParentTile->mChildrenTiles.size() > mTileIndex + 1) &&
        isMouseInBounds(mParentTile->mChildrenTiles.at(mTileIndex + 1))) {
        mParentTile->mChildrenTiles.at(mTileIndex + 1)->mJoinGuide->setVisibility(true);
        mJoinGuide->setVisibility(false);
        mParentTile->mJoinType = keepSecond;
    }
}

void ZTileViewController::updateHorizontalJoinGuide() {
    if (mParentTile != nullptr && isMouseInBounds(this) && (mParentTile->mChildrenTiles.size() > mTileIndex + 1) &&
        mParentTile->mChildrenTiles.at(mTileIndex + 1)->mJoinGuide->getVisibility()) {
        mJoinGuide->setVisibility(true);
        if (mParentTile->mChildrenTiles.size() > mTileIndex + 1) {
            mParentTile->mChildrenTiles.at(mTileIndex + 1)->mJoinGuide->setVisibility(false);
        }

        mParentTile->mJoinType = keepSecond;
    }

    if (mParentTile != nullptr && (mParentTile->mChildrenTiles.size() > mTileIndex + 1) && isMouseInBounds(
            mParentTile->mChildrenTiles.at(mTileIndex + 1))) {
        if (mParentTile->mChildrenTiles.size() > mTileIndex + 1) {
            mParentTile->mChildrenTiles.at(mTileIndex + 1)->mJoinGuide->setVisibility(true);
        }
        mJoinGuide->setVisibility(false);
        mParentTile->mJoinType = keepFirst;
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

void ZTileViewController::setTileIndex(int index) {
    mIndexLabel->setText(to_string(index));
    mTileIndex = index;
}

void ZTileViewController::onGlobalMouseUp(int key) {
    ZViewController::onGlobalMouseUp(key);
    setConsumeClicks(false);
    mDragType = noDrag;
    mJoinGuide->setVisibility(false);
}

/**
 * Remove children from controller and put it back in its default state
 */
void ZTileViewController::resetController() {
    if (mChildrenTiles.size() == 1) {
        mDropDown->selectItem(mChildrenTiles.at(0)->getIndexTag());
        setIndexTag(mChildrenTiles.at(0)->getIndexTag());
        ZViewController* content = mChildrenTiles.at(0)->mContent;
        mChildrenTiles.at(0)->removeSubView(content);
        removeSubView(mChildrenTiles.at(0));
        mChildrenTiles.erase(mChildrenTiles.begin() + 0);
        addSubView(content);
        mContent = content;
        mHandle->setVisibility(true);
        mDropDown->setVisibility(true);
        mHandle->bringToFront();
        mDropDown->bringToFront();

    }
}

//////////
/// Split tiles
void ZTileViewController::triggerSideSplit() {

    /////////////// Step 3: create views after split

    // Check if this needs to be a root tile
    if (mTileType != horizontalTile) {
        removeSubView(mContent);

        ZTileViewController* leftTile;
        ZTileViewController* rightTile;
        if (mChildrenTiles.empty()) {
            leftTile = new ZTileViewController(getResourcePath(),
                                               mControllerFactory, mNames, false, mContent);
            rightTile = new ZTileViewController(getResourcePath(),
                                                mControllerFactory, mNames, false, nullptr);

            leftTile->mParentTile = this;
            rightTile->mParentTile = this;

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

void ZTileViewController::triggerOverUnderSplit() {// Trigger split

    /////////////// Step 3: create views after split

    // Check if this needs to be a root tile
    if (mTileType != verticalTile) {
        removeSubView(mContent);

        ZTileViewController* leftTile;
        ZTileViewController* rightTile;
        if (mChildrenTiles.empty()) {
            leftTile = new ZTileViewController(getResourcePath(),
                                               mControllerFactory, mNames, false, mContent);
            rightTile = new ZTileViewController(getResourcePath(),
                                                mControllerFactory, mNames, false, nullptr);

            leftTile->mParentTile = this;
            rightTile->mParentTile = this;

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

//////////
/// Join tiles
void ZTileViewController::triggerSideJoinLeftToRight(int index) {
    if (mSplitType == sideBySide) {
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
        onWindowChange(getWindowWidth(), getWindowHeight());
        mDragType = noDrag;

    }
}

void ZTileViewController::triggerSideJoinRightToLeft(int index) {
    if (mSplitType == sideBySide) {
        mDragType = noDrag;

        removeSubView(mChildrenTiles.at(index));
        mChildrenTiles.erase(mChildrenTiles.begin() + (index));
        updateIndices();

        int leftSide = 0;
        if (mChildrenTiles.size() > index - 1) {
            auto previous = mChildrenTiles.at(index - 1);
            leftSide = previous->getOffsetX() + previous->getMaxWidth();
        }

        int width = fillParent;
        if (mChildrenTiles.size() > index + 1) {
            auto next = mChildrenTiles.at(index + 1);
            width = next->getOffsetX() - leftSide;
        }

        mChildrenTiles.at(index)->setXOffset(leftSide);
        mChildrenTiles.at(index)->setMaxWidth(width);

        resetController();

    }
}

void ZTileViewController::triggerJoinTopToBottom(int index) {
    if (mSplitType == overUnder) {
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

        onWindowChange(getWindowWidth(), getWindowHeight());
        mDragType = noDrag;
    }
}

void ZTileViewController::triggerJoinBottomToTop(int index) {
    if (mSplitType == overUnder) {
        mDragType = noDrag;

        removeSubView(mChildrenTiles.at(index));
        mChildrenTiles.erase(mChildrenTiles.begin() + (index));
        updateIndices();

        auto tileToResize = mChildrenTiles.at(index);

        int height = fillParent;

        if (index - 1 >= 0) {
            auto next = mChildrenTiles.at(index - 1);
            height = next->getOffsetY() - tileToResize->getOffsetY();
        }

        tileToResize->setMaxHeight(height);
        resetController();
    }
}

