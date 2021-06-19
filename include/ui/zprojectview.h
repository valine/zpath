//
// Created by Lukas Valine on 6/18/21.
//

#ifndef ZPATH_ZPROJECTVIEW_H
#define ZPATH_ZPROJECTVIEW_H


#include "zview.h"
#include "zscrollview.h"

class ZProjectView : ZScrollView {

    ZProjectView(ZView *parent, vector<string> names);

};


#endif //ZPATH_ZPROJECTVIEW_H
