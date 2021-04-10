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


int main(int argc, char* argv[]) {

    CasUtil::get().testCompute();
   // vector<ZViewController*> controller = {new ZPathViewController(argv), new ZPathViewController(argv)};

    auto* view3d = new ZPathViewController(argv);
    view3d->setName("3D Viewport");

    auto* nodeView = new NodeViewController(argv);
    nodeView->setName("Node Editor");

    auto* textEditor = new ZTextViewController(argv);
    textEditor->setName("Text Editor");

    auto* tiles = new ZTileViewController(argv, [argv](int index) {
        ZViewController *controller = nullptr;

        switch (index) {
            case 0: {
                controller = new ZPathViewController(argv);
                return controller;
            }
            case 1: {
                controller = new NodeViewController(argv);
                return controller;
            }
            case 2: {
                controller = new ZTextViewController(argv);
                return controller;
            } case 3: {
                controller = new ZCalculatorViewController(argv);
                return controller;
            }
            default: {
                controller = new ZViewController(argv);
                return controller;
            }
        }

    }, {"3D Viewport", "Node Editor", "Text Editor", "Calculator"}, true, nullptr);


   // tiles->triggerSideSplit(0.5, 0);
    ZApplication(tiles, "zpath", false, 1200, 800, "resources/icons/zpath.png");

    //ZTabbedViewController* tabs = new ZTabbedViewController(argv, {nodeView, textEditor, view3d});
   // ZApplication(tabs, "zpath", false, 1200, 800);
}
