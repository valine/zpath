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

    label->mTitle->setText(getFileName(item));
    label->mDropDown->setTitle("Format");
    label->mDropDown->setOnOpen([label, this](){
        vector<string> list = mGetCrumbs();
        vector<string> fileNames;
        int i = 0;
        for (string item : list) {
            fileNames.push_back(getFileName(list.at(i)));
            i++;
        }
        label->mDropDown->setItems(fileNames);
    });

    onWindowChange(getWindowWidth(), getWindowHeight());
}

string ZListView::getFileName(const string& s) {
    char sep = '/';

#ifdef _WIN32
    sep = '\\';
#endif

    size_t i = s.rfind(sep, s.length());
    if (i != string::npos) {
        string name = (s.substr(i+1, s.length() - i));

        if (name.substr(name.length() - 5, name.length()) == "zpath") {
            return name.substr(0, name.length() - 6);
        } else {
            return name;
        }
    }

    return("");
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
        auto* view = new ZListItemView(fillParent, 30, this);
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
