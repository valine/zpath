//
// Created by Lukas Valine on 6/18/21.
//

#include "ui/zprojectview.h"

ZProjectView::ZProjectView(ZView *parent, vector<string> names)  : ZScrollView(150, fillParent, parent) {

    setBackgroundColor(white);
    setElevation(1.0);
    setCornerRadius(vec4(5,1,1,1));
    setInnerViewHeight(200);

    for (auto name : names) {

        auto btn = new ZButton(name, this);
    }
}
