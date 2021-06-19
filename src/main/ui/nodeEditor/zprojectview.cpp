//
// Created by Lukas Valine on 6/18/21.
//

#include "ui/zprojectview.h"

ZProjectView::ZProjectView(ZView *parent, vector<string> names)  : ZScrollView(100, fillParent, parent) {

    setBackgroundColor(red);
}
