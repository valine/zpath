//
// Created by lukas on 1/31/22.
//

#ifndef ZPATH_ZLISTVIEW_H
#define ZPATH_ZLISTVIEW_H

#include <ui/zscrollview.h>
#include <queue>
#include <utility>
#include "ui/zdropdown.h"
#include "ui/zlistitemview.h"

class ZListView : public ZScrollView {
public:
    ZListView(ZView* parent);

    void addItem(string item);
    void setCrumbInterface(function<vector<string>()> in) {
        mGetCrumbs = std::move(in);
    }

private:

    ZListItemView * newView();
    queue<ZListItemView*> mRecycledViews;
    vector<ZListItemView*> mListViews;
    function<vector<string>()> mGetCrumbs;

    void removeItem(ZListItemView *view);
    void setItems(const vector<string>& items);

    string getFileName(const string &s);
};


#endif //ZPATH_ZLISTVIEW_H
