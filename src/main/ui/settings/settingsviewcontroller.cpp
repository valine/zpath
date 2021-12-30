//
// Created by Lukas Valine on 10/24/21.
//

#include <ui/zscrollview.h>
#include <ui/zradiobutton.h>
#include "ui/settingsviewcontroller.h"

void SettingsViewController::onCreate() {
    ZViewController::onCreate();

    auto* scrollView = new ZScrollView(fillParent, fillParent, this);
    auto* label = new ZLabel("Settings", scrollView);
    label->setMargin(vec4(5, 5, 5,5));


    // Dark mode
    auto* darkModeCb = new ZCheckbox("Dark Mode", scrollView);
    darkModeCb->setChecked(ZSettings::get().getColorMode() == ColorMode::dark);
    darkModeCb->setOnCheck([this](ZView* sender, bool isChecked){
        if (isChecked) {
            ZSettings::get().setColorMode(ColorMode::dark);
        } else {
            ZSettings::get().setColorMode(ColorMode::light);
        }
    });

    // Mouse wheel
    auto* mouseWheel = new ZRadioButton("Mouse Wheel", {"Zoom", "Scroll"}, scrollView);
    switch(ZSettings::get().getWheelMode()) {
        case (zoom):
            mouseWheel->setSelected(0);
            break;
        case (scroll):
            mouseWheel->setSelected(1);
            break;
    }
    mouseWheel->setMaxWidth(100);
    mouseWheel->setOnClick([this](ZView* sender, int index){
        switch (index) {
            case 0:
                ZSettings::get().setWheelMode(zoom);
                break;
            case 1:
                ZSettings::get().setWheelMode(scroll);
                break;
        }
    });

    // Mouse wheel
    auto* compDevice = new ZRadioButton("Computation Mode", {"CPU", "GLSL"}, scrollView);
    switch(ZSettings::get().getCompDevice()) {
        case (cpu):
            compDevice->setSelected(0);
            break;
        case (glsl):
            compDevice->setSelected(1);
            break;
    }
    compDevice->setMaxWidth(100);
    compDevice->setOnClick([this](ZView* sender, int index){
        switch (index) {
            case 0:
                ZSettings::get().setComputationDevice(cpu);
                break;
            case 1:
                ZSettings::get().setComputationDevice(glsl);
                break;
        }
    });

    // Update margins for linear layout view
    scrollView->getInnerView()->refreshMargins();
}
