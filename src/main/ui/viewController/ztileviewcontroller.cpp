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

    setOutlineType(WireType::outline);
    setOutlineColor(grey2);
    setBackgroundColor(grey1);

    int defaultController = 1;

    if (mParentTile != nullptr && getIndexTag() == -1) {
        defaultController = mParentTile->getIndexTag();
    } else if (getIndexTag() != -1) {
        defaultController = getIndexTag();
    }

    if (mContent == nullptr) {
        mContent = mControllerFactory(defaultController);
        mControllers.at(defaultController) = mContent;
    } else if (mParentTile != nullptr) {
        defaultController = mParentTile->getIndexTag();
    }

    setIndexTag(defaultController);

    int boarder = 0;
    mContent->setDrawingEnabled(false);
    mContent->setLineWidth(2);
    mContent->setYOffset(boarder);
    mContent->setXOffset(boarder);
    mContent->setMarginBottom(BOTTOM_MARGIN);
    addSubView(mContent);

    mHandle = new ZView(vec2(25), this);
    mHandle->setGravity(topRight);
    mHandle->setBackgroundImage(new ZTexture(getResourcePath() + "resources/icons/tile.png"));

    mDropDown = new ZDropDown(100, 300, mNames, this);
    mDropDown->setGravity(bottomLeft);
    mDropDown->setYOffset(1);
    mDropDown->setXOffset(boarder);
    mDropDown->selectItem(defaultController);

    mJoinGuide = new ZView(fillParent,fillParent, this);
    mJoinGuide->setBackgroundColor(vec4(0.1,0.1,0.1,0.4));
    mJoinGuide->setVisibility(false);

    mDropDown->setOnItemChange([this](int index){
        selectController(index);
    });

    mIndexLabel = new ZLabel("-1", this);
    mIndexLabel->setGravity(Gravity::bottomRight);
    mIndexLabel->setYOffset(2);
    mIndexLabel->setTextColor(grey);
    mIndexLabel->setMaxWidth(15);
}

void ZTileViewController::selectController(int index) {
    for (ZViewController* controller : mControllers) {
        if (controller != nullptr) {
            controller->setVisibility(false);
        }
    }

    if(mControllers.at(index) == nullptr) {
        mControllers.at(index) = mControllerFactory(index);
        mContent = mControllers.at(index);
        mContent->setDrawingEnabled(false);
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

            int thresholdV = 23;
            if (mSplitType == overUnder && (abs(tile->getBottom() - y) < thresholdV && mChildrenTiles.size() > tile->mTileIndex - 1) &&
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
                    triggerSideSplit(0, -1);
                } else if (delta.x > DRAG_THRESHOLD) {
                    mDragType = pendingHorizontalJoin;
                    updateHorizontalJoinGuide();
                    mJoinGuide->bringToFront();

                    if (mParentTile != nullptr && mParentTile->mChildrenTiles.size() > mTileIndex + 1) {
                        mParentTile->mChildrenTiles.at(mTileIndex + 1)->mJoinGuide->bringToFront();
                    }
                } else if (delta.y > DRAG_THRESHOLD) {
                    triggerOverUnderSplit(1, -1, true);
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
    int margin = TILE_MARGIN;
    if (mSplitType == sideBySide) {
        if (mDragIndex <= 0) {
            return;
        }

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
        if (mDragIndex <= 0) {
            return;
        }
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

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
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

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());

}

void ZTileViewController::insertChildAtIndexHorizontal(int index, int controllerType) {
    auto tile = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, nullptr);
    tile->setIndexTag(controllerType);
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

void ZTileViewController::insertChildAtIndexVertical(int index, int controllerType, float percent) {
    auto tile = new ZTileViewController(getResourcePath(), mControllerFactory, mNames, false, nullptr);
    tile->setIndexTag(controllerType);
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
    onTileEdgeDrag(vec2(0, mChildrenTiles.at(index - 1)->getHeight() * percent));

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
ZTileViewController* ZTileViewController::triggerSideSplit(float percent, int controllerIndex) {

    /////////////// Step 3: create views after split
    ZTileViewController* leftTile;
    ZTileViewController* rightTile;

    // Check if this needs to be a root tile
    if (mTileType != horizontalTile) {
        removeSubView(mContent);

        if (mChildrenTiles.empty()) {
            leftTile = new ZTileViewController(getResourcePath(),
                                               mControllerFactory,
                                               mNames, false, mContent);
            rightTile = new ZTileViewController(getResourcePath(),
                                                mControllerFactory,
                                                mNames, false, nullptr);
            leftTile->mParentTile = this;
            rightTile->mParentTile = this;

            rightTile->setIndexTag(controllerIndex);
            addSubView(leftTile);
            addSubView(rightTile);
        } else {
            leftTile = mChildrenTiles.at(0);
            rightTile = mChildrenTiles.at(1);
        }

        float leftPercent = 1.0 - percent;
        leftTile->setParentView(this);
        leftTile->setDrawingEnabled(false);
        leftTile->setVisibility(true);
        leftTile->setTileIndex(0);
        leftTile->mTileType = horizontalTile;
        leftTile->mParentTile = this;
        leftTile->setXOffset(0);
        leftTile->setMaxWidth(getWidth() * leftPercent);

        mChildrenTiles.push_back(leftTile);

        rightTile->setDrawingEnabled(false);
        rightTile->setVisibility(true);

        if (getWidth() * leftPercent + getWidth() * percent >= getWindowWidth()) {
            rightTile->setMaxWidth(fillParent);
        } else {
            rightTile->setMaxWidth(getWidth() * percent);
        }
        rightTile->setXOffset(getWidth() * leftPercent);
        rightTile->setParentView(this);
        rightTile->mParentTile = this;
        rightTile->setTileIndex(1);
        rightTile->mTileType = horizontalTile;

        mChildrenTiles.push_back(rightTile);

        // Controller index is -1 when user drags the corner
        mSplitType = sideBySide;
        if (controllerIndex == -1) {
            mDragType = tileDrag;
            mDragIndex = 1;
        }

        mInitialBondary = getWidth() * leftPercent;

        mHandle->setVisibility(false);
        mDropDown->setVisibility(false);

    } else {
        if (controllerIndex == -1) {
            mParentTile->insertChildAtIndexHorizontal(mTileIndex + 1, getIndexTag());
        } else {
            mParentTile->insertChildAtIndexHorizontal(mTileIndex + 1, controllerIndex);
        }
        mDragType = noDrag;
    }

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
    return rightTile;
}

ZTileViewController * ZTileViewController::triggerOverUnderSplit(float percent, int controllerIndex, bool enterDrag) {// Trigger split

    /////////////// Step 3: create views after split
    ZTileViewController* bottomTile;
    ZTileViewController* topTile;

    float margin = TILE_MARGIN;

    // Check if this needs to be a root tile
    if (mTileType != verticalTile) {
        removeSubView(mContent);

        if (mChildrenTiles.empty()) {
            bottomTile = new ZTileViewController(
                    getResourcePath(),mControllerFactory,
                    mNames, false, mContent);
            topTile = new ZTileViewController(
                    getResourcePath(),mControllerFactory,
                    mNames, false, nullptr);

            bottomTile->mParentTile = this;
            topTile->mParentTile = this;

            topTile->setIndexTag(controllerIndex);
            addSubView(bottomTile);
            addSubView(topTile);
        } else {
            bottomTile = mChildrenTiles.at(0);
            topTile = mChildrenTiles.at(1);
        }

        bottomTile->setParentView(this);
        bottomTile->setDrawingEnabled(false);
        // bottomTile->mHandle->bringToFront();
        bottomTile->setVisibility(true);
        bottomTile->setTileIndex(0);
        bottomTile->mTileType = verticalTile;
        bottomTile->mParentTile = this;

        float percentLeft = 1.0 - percent;
        if (getHeight() * percent + getHeight() * percentLeft >= getHeight()) {
            bottomTile->setMaxHeight(fillParent);
        } else {
            bottomTile->setMaxHeight(getHeight() * percent);
        }

        bottomTile->setYOffset(std::max(margin, getHeight() * percentLeft));
        mChildrenTiles.push_back(bottomTile);

        topTile->setDrawingEnabled(false);
        topTile->setVisibility(true);
        topTile->setMaxHeight(std::max(margin, getHeight() * percentLeft));
        topTile->setYOffset(0);
        topTile->setParentView(this);
        topTile->mParentTile = this;
        topTile->setTileIndex(1);
        // topTile->mHandle->bringToFront();
        topTile->mTileType = verticalTile;

        mChildrenTiles.push_back(topTile);

        mSplitType = overUnder;
        if (controllerIndex == -1) {
            mDragType = tileDrag;
            mDragIndex = 1;
            mInitialBondary = getHeight() * percentLeft;
        }

        mHandle->setVisibility(false);
        mDropDown->setVisibility(false);

    } else {
        if (controllerIndex == -1) {
            mParentTile->insertChildAtIndexVertical(mTileIndex + 1, getIndexTag(), percent);
        } else {
            mParentTile->insertChildAtIndexVertical(mTileIndex + 1, controllerIndex, percent);
        }

        if (!enterDrag) {
            mParentTile->mDragType = noDrag;
        }
        mDragType = noDrag;
        mDragIndex = -1;
    }

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
    return topTile;
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
                offset = mChildrenTiles.at(index + 1)->getOffsetY() +
                        mChildrenTiles.at(index + 1)->getMaxHeight();
            }

            if (index - 1 >= 0) {
                height =
                        mChildrenTiles.at(index - 1)->getOffsetY() - offset;
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

ZTileViewController* ZTileViewController::getChild(int index) {
    return mChildrenTiles.at(index);
}
