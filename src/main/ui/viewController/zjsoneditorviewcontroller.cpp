//
// Created by lukas on 3/19/22.
//

#include "ui/zjsoneditorviewcontroller.h"
#include <ui/zlineview.h>
#include <ui/znodeeditor.h>
#include <utils/zshadowrenderer.h>

void ZJsonEditorViewController::onCreate() {
    ZViewController::onCreate();


    ZViewController::onCreate();

    auto* nodeView = new ZNodeEditor(fillParent, fillParent, this);
}


