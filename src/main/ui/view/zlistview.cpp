//
// Created by lukas on 1/31/22.
//

#include "ui/zlistview.h"

ZListView::ZListView(ZView *parent)  :
        ZScrollView(fillParent, fillParent, parent){
    setBackgroundColor(grey0);
}

void ZListView::addItem(string item) {
    ZLabel* label = newView();
    label->setText(item);
    onWindowChange(getWindowWidth(), getWindowHeight());

    label->setOnClick([this](ZView* sender){
        for (ZView* view: mListViews){
            view->setBackgroundColor(transparent);
        }
        sender->setBackgroundColor(yellow);
    });
}

void ZListView::setItems(const vector<string>& items) {
    for (ZLabel* view : mListViews) {
        removeItem(view);
    }
    for (const string& item : items) {
        addItem(item);
    }
}

void ZListView::removeItem(ZLabel* view) {
    if (view->getVisibility()) {
        view->setVisibility(false);
        mRecycledViews.push(view);
    }
}

ZLabel * ZListView::newView() {
    if (mRecycledViews.empty()) {
        auto* view = new ZLabel("", this);
        mListViews.push_back(view);
        return view;
    } else {
        ZLabel* view = mRecycledViews.front();
        mRecycledViews.pop();
        view->setVisibility(true);
        subviewToBack(view);
        return view;
    }
}
