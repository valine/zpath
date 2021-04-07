#include <utility>

#include <utility>

//
// Created by lukas on 3/10/21.
//

#include "ui/zdrawer.h"

ZDrawer::ZDrawer(ZView *parent, vector<string> names, vector<vec4> colors) : ZScrollView(100, fillParent, parent) {
    setBackgroundColor(bg);
    setElevation(0.5);

    mNames = std::move(names);
    mColors = std::move(colors);

    int buttonIndex = 0;
    for (const string& name : mNames) {
        auto* button = new ZButton(name, this);
        mButtons.push_back(button);

        button->setBackgroundColor(mColors.at(buttonIndex));
        button->setHighlighColor(mColors.at(buttonIndex) + vec4(0.1,0.1,0.1,0.0));
        button->setMarginTop(BUTTON_MARGIN);
        button->setMarginLeft(BUTTON_MARGIN);
        button->setMarginRight(BUTTON_MARGIN);
        button->setCornerRadius(vec4(0,button->getMaxHeight() / 2,
                                     button->getMaxHeight() / 2, 0));
        buttonIndex++;
    }

    getInnerView()->refreshMargins();
   
}

void ZDrawer::onCursorPosChange(double x, double y) {
    ZScrollView::onCursorPosChange(x,y);

    if (mSelected != -1 && getMouseDragDelta().x > 10) {
        if (mOnItemSelected != nullptr) {
            mOnItemSelected(mSelected);
        }
        mSelected = -1;
    }
}

void ZDrawer::onGlobalMouseUp(int key) {
    ZScrollView::onGlobalMouseUp(key);
    if (mSelected != -1 && getMouseDragDelta().x < 10) {
        if (mOnItemClicked != nullptr) {
            mOnItemClicked(mSelected);
        }
    }
    mSelected = -1;
}
void ZDrawer::onMouseEvent(int button, int action, int mods, int x, int y) {
    ZScrollView::onMouseEvent(button, action, mods, x, y);
    if (action == GLFW_PRESS) {
        int index = 0;
        for (ZView* view : mButtons) {
            if (isMouseInBounds(view)) {
                mSelected = index;
            }
            index++;
        }
    }

}