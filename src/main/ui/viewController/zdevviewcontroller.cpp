//
// Created by Lukas Valine on 6/23/21.
//

#include <utils/zgridrenderer.h>
#include "ui/zdevviewcontroller.h"
#include "utils/zfontstore.h"

ZDevViewController::ZDevViewController(char **const pString) : ZViewController(pString) {

}

void ZDevViewController::onCreate() {
    ZViewController::onCreate();


//    auto* view = new ZView(100, 100, this);
//    view->setBackgroundColor(red);
//    view->setDraggable(true);
//    view->setMargin(5);



    ZFontStore::getInstance().loadFont(ZFontStore::getInstance().getDefaultResource(), 2, 14);
    mLabel = new ZLabel("Hello", this);
    mLabel->setVisibility(false);
    mLabel->setText("Testing invisible text change");
    mLabel->setMargin(100);
    //mLabel->setVisibility(true);

//    ZGridRenderer renderer = ZGridRenderer::get();
//    auto tex = renderer.create();
//    view->setBackgroundImage(tex);

}

void ZDevViewController::draw() {
    ZViewController::draw();
    mLabel->setVisibility(true);

}