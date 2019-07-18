//
// Created by lukas on 7/18/19.
//

#ifndef ZPATH_ZTABVIEW_H
#define ZPATH_ZTABVIEW_H
using namespace std;

#include "zview.h"
#include "ui/zbutton.h"
template<typename V>
class ZTabView : public ZView, public ZOnClickListener {
public:
    ZTabView<V>(float maxWidth1, float maxHeight1,string resourcePath, vector<string> tabs);
    void onClick(ZButton* sender);
    void addToTab(ZView* view, int tabIndex);
private:
    vector<ZView*> mViews;
    map<ZButton*, ZView*> mTabMap;
};


#endif //ZPATH_ZTABVIEW_H
