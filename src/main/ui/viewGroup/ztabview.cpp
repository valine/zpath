//
// Created by lukas on 7/18/19.
//

#include <ui/zradiobutton.h>
#include <ui/z3dview.h>
#include <ui/zlinearlayout.h>
#include <ui/zscrollview.h>
#include "ui/ztabview.h"
ZTabView::ZTabView(float maxWidth1, float maxHeight1, const vector<ZView*>& tabs, ZView* parent) : ZView(maxWidth1, maxHeight1, parent) {
    vector<string> names;
    for (ZView* tab : tabs) {
        names.push_back(tab->getName());
        tab->setBackgroundColor(grey);
        tab->setVisibility(false);
        tab->setMarginBottom(25);
    }

    tabs.at(0)->setVisibility(true);

    for (ZView* view : tabs) {
        addSubView(view);
    }

    auto* tabBar = new ZRadioButton("", names, this);
    tabBar->setGravity(ZView::bottomLeft);
    tabBar->setText("");
    tabBar->setMargin(0,0,0,0);
    tabBar->setOnClick([this](ZView* sender, int index){
        for (ZView* view : mViews) {
            view->setVisibility(false);
        }
        mViews.at(index)->setVisibility(true);
    });


    mViews = tabs;

    invalidate();
}
