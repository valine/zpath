//
// Created by lukas on 7/18/19.
//

#include <ui/zradiobutton.h>
#include <ui/z3dview.h>
#include <ui/zlinearlayout.h>
#include <ui/zscrollview.h>
#include "ui/ztabview.h"
template<typename V>
ZTabView<V>::ZTabView(float maxWidth1, float maxHeight1, string resourcePath, vector<string> tabs) : ZView(maxWidth1, maxHeight1) {
    setBackgroundColor(vec4(1,0,0,1));
    ZRadioButton* tabBar = new ZRadioButton("", tabs, this);
    tabBar->setGravity(ZView::bottomLeft);
    tabBar->setText("");
    tabBar->setOnClickListener(this);

    for (uint i = 0; i < tabs.size(); i++) {
        V *tab = new V(maxWidth1, maxHeight1);
        auto fi = (double) i;
        tab->setBackgroundColor(vec4(fi / 3.0,0.0,0.0,1.0));
        addSubView(tab);
        mViews.push_back(tab);
        mTabMap.insert(make_pair(tabBar->getButton(i), tab));
    }

    onClick(tabBar->getButton(0));
    addSubView(tabBar);
}

template<typename V>
void ZTabView<V>::onClick(ZButton *sender) {
    ZOnClickListener::onClick(sender);
    for(int i = 0; i < mViews.size(); i++) {
        mViews.at(i)->setVisibility(false);
    }
    mTabMap.at(sender)->setVisibility(true);
    mTabMap.at(sender)->invalidate();
}

template<typename V>
void ZTabView<V>::addToTab(ZView *view, int tabIndex) {
    mViews.at(tabIndex)->addSubView(view);
}

template class ZTabView<ZView>;
template class ZTabView<ZLinearLayout>;
template class ZTabView<ZScrollView>;