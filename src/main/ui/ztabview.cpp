//
// Created by lukas on 7/18/19.
//

#include <ui/zradiobutton.h>
#include "ui/ztabview.h"
template<typename V>
ZTabView<V>::ZTabView(float maxWidth1, float maxHeight1, string resourcePath, vector<string> tabs) : ZView(maxWidth1, maxHeight1) {
   // typedef typename V::ZView sub_t;

    setBackgroundColor(vec4(1,0,0,1));
    ZRadioButton* tabBar = new ZRadioButton(maxWidth1, 40, resourcePath, tabs);
    tabBar->setGravity(ZView::bottomLeft);
    tabBar->setText("");
    addSubView(tabBar);

    V* tabView = new V(maxWidth1, maxHeight1);


}


template class ZTabView<ZView>;