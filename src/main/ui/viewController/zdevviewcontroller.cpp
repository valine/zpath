//
// Created by Lukas Valine on 6/23/21.
//

#include <utils/zgridrenderer.h>
#include "ui/zdevviewcontroller.h"

ZDevViewController::ZDevViewController(char **const pString) : ZViewController(pString) {

}



void ZDevViewController::onCreate() {
    ZViewController::onCreate();

    auto* view = new ZView(100, 100, this);
    view->setBackgroundColor(gold);
    view->setMargin(5);

    ZGridRenderer renderer = ZGridRenderer::get();

    auto tex = renderer.createShadow(100, 100, 5);
    view->setBackgroundImage(tex);

}

