//
// Created by lukas on 7/18/19.
//

#ifndef ZPATH_ZTABVIEW_H
#define ZPATH_ZTABVIEW_H


#include "zview.h"
template<typename V>
class ZTabView : public ZView {
public:
    ZTabView<V>(float maxWidth1, float maxHeight1,string resourcePath, vector<string> tabs);
};


#endif //ZPATH_ZTABVIEW_H
