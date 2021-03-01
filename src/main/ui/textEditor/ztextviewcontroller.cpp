//
// Created by lukas on 2/28/21.
//

#include <ui/ztextfield.h>
#include "zpath/ztextviewcontroller.h"

void ZTextViewController::onCreate() {
    ZViewController::onCreate();

    ZTextField* textField = new ZTextField(this);
    textField->setMaxHeight(ZView::fillParent);
    textField->setMargin(vec4(10));
    textField->setXOffset(0);


}
