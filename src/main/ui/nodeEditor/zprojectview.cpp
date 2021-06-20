//
// Created by Lukas Valine on 6/18/21.
//

#include "ui/zprojectview.h"
#include "ui/ztextfield.h"
ZProjectView::ZProjectView(ZView *parent, vector<string> names)  : ZScrollView(120, fillParent, parent) {

    setBackgroundColor(white);
    setElevation(1.0);
    setCornerRadius(vec4(5,1,1,1));
    setInnerViewHeight(200);

    for (const auto& name : names) {
        auto tf = new ZTextField(this);
        tf->setText(getFileName(name));
        tf->setFocusMode(ZTextField::FocusMode::doubleClick);
        mButtons.push_back(tf);
        tf->setMaxWidth(200);
        tf->setCornerRadius(vec4(5));
        tf->setMargin(vec4(2,2,2,0));
        tf->setBackgroundColor(white);
        tf->setOnClick([this](ZView* sender){
            for (auto otherTf : mButtons) {
                otherTf->setBackgroundColor(white);
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