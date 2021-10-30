//
// Created by Lukas Valine on 10/24/21.
//

#include <ui/zscrollview.h>
#include "ui/settingsviewcontroller.h"

void SettingsViewController::onCreate() {
    ZViewController::onCreate();

    auto* scrollView = new ZScrollView(fillParent, fillParent, this);
    auto* label = new ZLabel("Settings", scrollView);
    label->setMargin(vec4(5, 5, 5,5));

    auto* darkModeCb = new ZCheckbox("Dark Mode", scrollView);
    darkModeCb->setChecked(ZSettingsStore::get().getColorMode() == ColorMode::dark);
    darkModeCb->setOnCheck([this](ZView* sender, bool isChecked){
        if (isChecked) {
            ZSettingsStore::get().setColorMode(ColorMode::dark);
        } else {
            ZSettingsStore::get().setColorMode(ColorMode::light);
        }
    });

    scrollView->getInnerView()->refreshMargins();
}
