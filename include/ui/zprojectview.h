//
// Created by Lukas Valine on 6/18/21.
//

#ifndef ZPATH_ZPROJECTVIEW_H
#define ZPATH_ZPROJECTVIEW_H


#include "zview.h"
#include "zscrollview.h"
#include "ui/ztextfield.h"

class ZProjectView : public ZScrollView {

public :
    ZProjectView(ZView *parent, vector<string> names);
    string getFileName(const string &s);

private:
    vector<ZTextField*> mButtons;
};


#endif //ZPATH_ZPROJECTVIEW_H
