//
// Created by lukas on 3/10/21.
//

#ifndef ZPATH_ZTILEVIEWCONTROLLER_H
#define ZPATH_ZTILEVIEWCONTROLLER_H


static const int DRAG_THRESHOLD = 5;

#include <ui/zviewcontroller.h>

class ZTileViewController : public ZViewController {
public:

    ZTileViewController(char **argv, std::function<ZViewController *(int)> factory, vector<string> names);
    ZTileViewController(string path, std::function<ZViewController *(int)> factory, vector<string> names);


    int sideBySide = 0;
    int overUnder = 1;

    int cornerDrag = 0;
    int tileDrag = 1;
    int noDrag = -1;
private:

    int mDragType = noDrag;
    int mSplitType = sideBySide;

    int mInitialFirst = 0;
    int mInitialSecond = 0;

    ZTileViewController* mParentTile = nullptr;

    std::function<ZViewController*(int)> mControllerFactory = nullptr;
    vector<string> mNames;
    void onLayoutFinished() override;

    bool onMouseEvent(int button, int action, int mods, int x, int y) override;
    void onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) override;
    void onGlobalMouseUp(int key) override;

    ZView* mHandle = nullptr;

    ZViewController* mFirstView = nullptr;
    ZTileViewController* mSecondView = nullptr;
    vector<ZTileViewController*> mSecondViews;
    vector<int> mInitialSeconds;

    void triggerSideSplit();

    void triggerOverUnderSplit();

    void setParentTile(ZTileViewController *tileView);
};


#endif //ZPATH_ZTILEVIEWCONTROLLER_H
