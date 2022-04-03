#include "main.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;

#include <fstream>
#include <string>
#include <signal.h>
#include <stdlib.h>
#include <ui/zapplication.h>
#include <ui/zviewcontroller.h>
#include <zpath/zpathviewcontroller.h>
#include <ztabbedviewcontroller.h>
#include <zpath/nodeviewcontroller.h>
#include <utils/casutil.h>
#include <ui/ztextfield.h>
#include <zpath/ztextviewcontroller.h>
#include <ui/ztileviewcontroller.h>
#include <ui/zcalculatorviewcontroller.h>
#include <ui/zdevviewcontroller.h>
#include <ui/settingsviewcontroller.h>
#include <ui/zdataviewcontroller.h>
#include <ui/zjsoneditorviewcontroller.h>

int main(int argc, char* argv[]) {

    CasUtil::get().testCompute();
   // vector<ZViewController*> controller = {new ZPathViewController(argv), new ZPathViewController(argv)};
//
//    auto* view3d = new ZPathViewController(argv);
//    view3d->setName("3D Viewport");
//
//    auto* nodeView = new NodeViewController(argv);
//    nodeView->setName("Node Editor");
//
//    auto* textEditor = new ZTextViewController(argv);
//    textEditor->setName("Text Editor");

    auto* tiles = new ZTileViewController(argv, [argv](int index) {
        ZViewController *controller = nullptr;

        switch (index) {
            case 0: {
                controller = new ZPathViewController(argv);
                return controller;
            }
            case 7: {
                controller = new NodeViewController(argv);
                return controller;
            }
            case 2: {
                controller = new ZTextViewController(argv);
                return controller;
            } case 3: {
                controller = new ZCalculatorViewController(argv);
                return controller;
            } case 4: {
                controller = new ZDevViewController(argv);
                return controller;
            } case 5: {
                controller = new SettingsViewController(argv);
                return controller;
            } case 6: {
                controller = new ZDataViewController(argv);
                return controller;
            } case 1: {
                controller = new ZJsonEditorViewController(argv);
                return controller;
            }
            default: {
                controller = new ZViewController(argv);
                return controller;
            }
        }

    }, {"3D Viewport", "Node Editor", "Text Editor", "Calculator", "Developer", "Settings", "Data Import", "JSON Editor"}, true, nullptr);

    ZApplication(tiles, "zpath", false, 2100, 1000, "resources/icons/zpath.png", [tiles](){

        // Calculator
        auto calculator = tiles->triggerSideSplit(0.25, 3);
        auto viewport = calculator->triggerOverUnderSplit(0.5, 2, false);
        auto dev = calculator->getChild(0)->triggerOverUnderSplit(0.5, 7, false);

        auto data = calculator->getChild(0)->triggerOverUnderSplit(0.5, 6, false);

        auto textedit = viewport->triggerOverUnderSplit(0.5, 0, false);
        auto settings = tiles->getChild(0)->triggerOverUnderSplit(1.0, 5, false);

    });

    //ZTabbedViewController* tabs = new ZTabbedViewController(argv, {nodeView, textEditor, view3d});
   // ZApplication(tabs, "zpath", false, 1200, 800);
}
