//
// Created by lukas on 3/10/21.
//

#ifndef ZPATH_ZTILEVIEWCONTROLLER_H
#define ZPATH_ZTILEVIEWCONTROLLER_H


static const int DRAG_THRESHOLD = 5;

#include <ui/zviewcontroller.h>
#include "zdropdown.h"

class ZTileViewController : public ZViewController {
public:

    ZTileViewController(char **argv, std::function<ZViewController *(int)> factory,
                        vector<string> names, bool isRoot, ZViewController *content);
    ZTileViewController(string path, std::function<ZViewController *(int)> factory,
                        vector<string> names, bool isRoot, ZViewController *content);


    int sideBySide = 0;
    int overUnder = 1;

    int cornerDrag = 0;
    int tileDrag = 1;
    int noDrag = -1;
    int pendingHorizontalJoin = 6;
    int pendingVerticalJoin = 7;

    int horizontalTile = 0;
    int verticalTile = 1;
    int rootTile = 2;

    int keepFirst = 9;
    int keepSecond = 10;
private:

    bool mIsRoot = false;
    int mDragType = noDrag;
    int mSplitType = sideBySide;
    int mDragIndex = 0;
    int mJoinType = keepFirst;

    int mInitialBondary = 0;
    int mInitialSecond = 0;

    int mTileType = rootTile;

    int mTileIndex;
    ZLabel* mIndexLabel;

    std::function<ZViewController*(int)> mControllerFactory = nullptr;
    vector<string> mNames;
    vector<ZViewController*> mControllers;

    void onLayoutFinished() override;

    void onMouseEvent(int button, int action, int mods, int x, int y) override;
    void onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) override;
    void onGlobalMouseUp(int key) override;

    ZView* mHandle = nullptr;
    ZDropDown* mDropDown = nullptr;
    ZView* mJoinGuide;

    vector<ZTileViewController*> mChildrenTiles;
//    ZTileViewController* mLeftTile = nullptr;
//    ZTileViewController* mRightTile = nullptr;
    ZTileViewController* mParentTile = nullptr;
    ZViewController* mContent = nullptr;

    void triggerSideSplit();
    void triggerSideJoinLeftToRight(int index);
    void triggerSideJoinRightToLeft(int index);
    void triggerOverUnderSplit();


    void setTileIndex(int index);

    void insertChildAtIndexHorizontal(int index);

    void updateIndices();

    void insertChildAtIndexVertical(int index);

    void triggerJoinBottomToTop(int index);

    void resetController();

    void triggerJoinTopToBottom(int index);

    void updateHorizontalJoinGuide();

    void updateVerticalJoinGuide();
};


#endif //ZPATH_ZTILEVIEWCONTROLLER_H
