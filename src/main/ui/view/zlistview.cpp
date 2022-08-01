//
// Created by lukas on 1/31/22.
//

#include "ui/zlistview.h"
#include "ui/zlistitemview.h"
#include <utility>
#include <thread>

ZListView::ZListView(ZView *parent)  :
        ZScrollView(fillParent, fillParent, parent){
    setBackgroundColor(grey0);
}

void ZListView::addItem(string item) {
    ZListItemView* label = newView();

    mItems.push_back(item);

    label->mTitle->setText(getFileName(item));
    label->mDropDown->setTitle("Format");
    label->mDropDown->setIndexTag(mListViews.size() - 1);
    label->mDropDown->setOnOpen([label, this](){
        updateNames(label);

        if (getItemColor != nullptr) {
            int buttonIndex = 0;
            for (auto button : label->mDropDown->mButtons) {
                thread t1(&ZListView::setItemColor, this, label, button, buttonIndex);
                t1.detach();

                buttonIndex++;
            }
        }
    });
    label->mDropDown->setOnItemChange([this, label](int item){
        if (mCrumbChangeListener != nullptr) {
            mCrumbChangeListener(label->mDropDown->getIndexTag(), item);
        }
    });

    onWindowChange(getWindowWidth(), getWindowHeight());
}

void ZListView::updateNamesAtIndex(int index) {
    updateNames(mListViews.at(index));
}

void ZListView::updateNames(ZListItemView *label) {
    vector<string> list = mGetCrumbs();
    vector<string> fileNames;
    int i = 0;
    for (string item : list) {
        fileNames.push_back(getFileName(list.at(i)));
        i++;
    }

    label->mDropDown->setItems(fileNames);
}

void ZListView::setItemDropDown(vector<string> items, int index) {
    mListViews.at(index)->mDropDown->setItems(std::move(items));
}

void ZListView::selectItemDropDown(int itemIndex, int index) {
    mListViews.at(itemIndex)->mDropDown->selectItem(index);
}

vector<int> ZListView::getFormats() {
    vector<int> formats;
    for (auto view : mListViews) {
        formats.push_back(view->mDropDown->getSelectedItem());
    }

    return formats;
}

void ZListView::setItemColor(ZListItemView *label, ZButton *button, int index) {
    button->setBackgroundColor(
            getItemColor(index, label->mDropDown->getIndexTag()));
    button->invalidate();
    glfwPostEmptyEvent();
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
