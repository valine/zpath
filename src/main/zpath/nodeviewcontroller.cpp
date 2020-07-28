//
// Created by lukas on 7/27/20.
//

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
}

