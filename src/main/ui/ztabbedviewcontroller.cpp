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
    setBackgroundColor(ZSettingsStore::getInstance().getBackgroundColor());
}

void ZTabbedViewController::onCreate() {
    ZViewController::onCreate();

    auto* controllerHolder = new ZView(fillParent, fillParent, this);
    controllerHolder->setMargin(0,0,0,25);
    controllerHolder->setBackgroundColor(vec4(1,0,0,1));

    vector<string> names;
    for (ZViewController* controller : mControllers) {
        names.push_back(controller->getName());
    }

    for (ZViewController* controller : mControllers) {
        controllerHolder->addSubView(controller);
        controller->setVisibility(false);
        controller->invalidate();
    }
    mControllers.at(0)->setVisibility(true);

    ZRadioButton* tabBar = new ZRadioButton("", names, this);
    tabBar->showLabel(false);
    tabBar->setGravity(ZView::bottomLeft);
    tabBar->setBackgroundColor(ZSettingsStore::getInstance().getBackgroundColor());
    tabBar->setText("");
    tabBar->setMargin(0,0,0,0);
    tabBar->setMaxWidth(100 * mControllers.size());
    tabBar->setMaxHeight(25);
    tabBar->setOnClick([this](ZView*, int index){
        for (ZViewController* controller : mControllers) {
            controller->setVisibility(false);
        }
        mControllers.at(index)->setVisibility(true);
        mControllers.at(index)->invalidate();
    });

}

