//
// Created by lukas on 1/31/22.
//

#ifndef ZPATH_ZLISTVIEW_H
#define ZPATH_ZLISTVIEW_H

#include <ui/zscrollview.h>
#include <queue>

class ZListView : public ZScrollView {
public:
    ZListView(ZView* parent);

    void addItem(string item);

private:

    ZLabel * newView();
    queue<ZLabel*> mRecycledViews;
    vector<ZLabel*> mListViews;

    void removeItem(ZLabel *view);

    void setItems(const vector<string>& items);
};


#endif //ZPATH_ZLISTVIEW_H
