//
// Created by lukas on 7/18/19.
//

#ifndef ZPATH_ZTABVIEW_H
#define ZPATH_ZTABVIEW_H
using namespace std;

#include "zview.h"
#include "ui/zbutton.h"

class ZTabView : public ZView {
public:
    ZTabView(float maxWidth1, float maxHeight1, const vector<ZView*>& tabs, ZView* parent);
private:
    vector<ZView*> mViews;
};


#endif //ZPATH_ZTABVIEW_H
