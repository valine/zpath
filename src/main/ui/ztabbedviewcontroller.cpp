#include <utility>

//
// Created by lukas on 7/26/20.
//

#include <ui/zscrollview.h>
#include <ui/ztabview.h>
#include <ui/zradiobutton.h>
#include "ztabbedviewcontroller.h"


ZTabbedViewController::ZTabbedViewController(char **argv, vector<ZViewController *> controllers) : ZViewController(argv){
    mControllers = std::move(controllers);
    for (ZViewController* controller : mControllers) {
        controller->setDrawingEnabled(false);
    }
}

ZTabbedViewController::ZTabbedViewController(string path, vector<ZViewController *> controllers) : ZViewController(path) {
    mControllers = std::move(controllers);
}

void ZTabbedViewController::onCreate() {
    ZViewController::onCreate();

    vector<string> names;
    for (ZViewController* controller : mControllers) {
        names.push_back(controller->getName());
    }

    for (ZViewController* controller : mControllers) {
        addSubView(controller);
        controller->setVisibility(false);
    }
    mControllers.at(0)->setVisibility(true);

    ZRadioButton* tabBar = new ZRadioButton("", names, this);
    tabBar->setGravity(ZView::bottomLeft);
    tabBar->setText("");
    tabBar->setMaxWidth(100 * mControllers.size());
    tabBar->setOnClick([this](ZView*, int index){
        for (ZViewController* controller : mControllers) {
            controller->setVisibility(false);
        }
        mControllers.at(index)->setVisibility(true);
    });

}

