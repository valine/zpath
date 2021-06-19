//
// Created by Lukas Valine on 6/18/21.
//

#include "ui/zprojectview.h"

ZProjectView::ZProjectView(ZView *parent, vector<string> names)  : ZScrollView(120, fillParent, parent) {

    setBackgroundColor(white);
    setElevation(1.0);
    setCornerRadius(vec4(5,1,1,1));
    setInnerViewHeight(200);

    for (const auto& name : names) {
        auto btn = new ZButton(getFileName(name), this);
        mButtons.push_back(btn);
        btn->setMaxWidth(200);
        btn->setCornerRadius(vec4(5));
        btn->setMargin(vec4(2,2,2,0));
        btn->setBackgroundColor(white);
        btn->setOnClick([this](ZButton* sender){
            for (auto otherBtn : mButtons) {
                otherBtn->setBackgroundColor(white);
            }
            sender->setBackgroundColor(highlight);
        });
    }

    getInnerView()->refreshMargins();
}

string ZProjectView::getFileName(const string& s) {
    char sep = '/';

#ifdef _WIN32
    sep = '\\';
#endif

    size_t i = s.rfind(sep, s.length());
    if (i != string::npos) {
        string name = (s.substr(i+1, s.length() - i));

        if (name.substr(name.length() - 5, name.length()) == "zpath") {
            return name.substr(0, name.length() - 6);
        }
    }

    return("");
}