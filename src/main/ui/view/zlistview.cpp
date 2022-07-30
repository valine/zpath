//
// Created by lukas on 1/31/22.
//

#include "ui/zlistview.h"
#include "ui/zlistitemview.h"
#include <utility>

ZListView::ZListView(ZView *parent)  :
        ZScrollView(fillParent, fillParent, parent){
    setBackgroundColor(grey0);
}

void ZListView::addItem(string item) {
    ZListItemView* label = newView();

//    label->setTitle(std::move(item));
    label->mDropDown->setOnOpen([label, this](){
        vector<string> list = mGetCrumbs();
        label->mDropDown->setItems(list);
    });

    onWindowChange(getWindowWidth(), getWindowHeight());
//
//    label->setOnClick([this](ZView* sender){
////        for (ZView* view: mListViews){
////            view->setBackgroundColor(transparent);
////        }
////        sender->setBackgroundColor(yellow);
//    });
}

void ZListView::setItems(const vector<string>& items) {
    for (ZListItemView* view : mListViews) {
        removeItem(view);
    }
    for (const string& item : items) {
        addItem(item);
    }
}

void ZListView::removeItem(ZListItemView* view) {
    if (view->getVisibility()) {
        view->setVisibility(false);
        mRecycledViews.push(view);
    }
}

ZListItemView * ZListView::newView() {
    if (mRecycledViews.empty()) {
        auto* view = new ZListItemView(400, 50, this);
        mListViews.push_back(view);
        return view;
    } else {
        ZListItemView* view = mRecycledViews.front();
        mRecycledViews.pop();
        view->setVisibility(true);
        subviewToBack(view);
        return view;
    }
}
