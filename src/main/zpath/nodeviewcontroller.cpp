//
// Created by lukas on 7/27/20.
//

#include <ui/zlineview.h>
#include <ui/zdropdown.h>
#include <ui/znodeeditor.h>
#include <ui/zradiobutton.h>
#include <utils/zshadowrenderer.h>
#include "zpath/nodeviewcontroller.h"

/**
 * Called after views are initialized
 */
void NodeViewController::onCreate() {
    ZViewController::onCreate();

    setBackgroundColor(vec4(0.1,0.1,0.1,1));

    // Button example
    auto* button = new ZButton("Button name", this);
    button->setOffset(2000,0);
    button->setOnClick([this](ZView*){
        // Do stuff here
    });

    auto* nodeView = new ZNodeEditor(fillParent, fillParent, this);

//    ZDropDown* dropDown = new ZDropDown(100,500, {"hello1", "hello2", "hello3", "hello4", "hello5"}, this);
//    dropDown->setOffset(500, 0);
//    ZDropDown* dropDown2 = new ZDropDown(100,500, {"hello1", "hello2", "hello3", "hello4", "hello5", "hello4", "hello7", "hello8"}, this);
//    dropDown2->setOffset(200, 0);


    //ZShadowRenderer shadowRenderer = ZShadowRenderer::get();
    //ZTexture* shadow = shadowRenderer.createShadow(200, 100, 10);

    //auto* myView = new ZView(200, 100, this);
    //myView->setBackgroundColor(blue);
    //myView->setBackgroundImage(shadow);

}

