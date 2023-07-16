//
// Created by lukas on 2/28/21.
//

#include <ui/ztextfield.h>
#include "zpath/ztextviewcontroller.h"

void ZTextViewController::onCreate() {
    ZViewController::onCreate();

    auto* textField = new ZTextField(this);
    textField->setMaxHeight(ZView::fillParent);
    textField->setMargin(vec4(3));
    textField->setBackgroundColor(bg);
    textField->setXOffset(0);
    textField->setCornerRadius(2);
    textField->setTextSize(30);
}
