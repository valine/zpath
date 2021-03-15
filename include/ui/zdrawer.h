#include <utility>

//
// Created by lukas on 3/10/21.
//

#ifndef ZPATH_ZDRAWER_H
#define ZPATH_ZDRAWER_H


static const int BUTTON_MARGIN = 3;

#include "zview.h"
#include "zscrollview.h"

class ZDrawer : public ZScrollView {

public:
    ZDrawer(ZView *parent, vector<string> names, vector<vec4> colors);


    vector<string> mNames;
    vector<vec4> mColors;
    vector<ZView*> mButtons;
    bool mActionTriggered = false;
    int mSelected = -1;

    void onMouseEvent(int button, int action, int mods, int x, int y);

    void setOnItemSelected(function<void(int)> listener) {
        mOnItemSelected = std::move(listener);
    }

    void setOnItemClicked(function<void(int)> listener) {
        mOnItemClicked = listener;
    }

private:

    function<void(int)> mOnItemSelected;

    function<void(int)> mOnItemClicked;
    void onCursorPosChange(double x, double y);

    void onGlobalMouseUp(int key);
};


#endif //ZPATH_ZDRAWER_H
