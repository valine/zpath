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


int main(int argc, char* argv[]) {

    CasUtil::get().testCompute();
   // vector<ZViewController*> controller = {new ZPathViewController(argv), new ZPathViewController(argv)};

    auto* view3d = new ZPathViewController(argv);
    view3d->setName("3D Viewport");

    auto* nodeView = new NodeViewController(argv);
    nodeView->setName("Node Editor");

    auto* textEditor = new ZTextViewController(argv);
    textEditor->setName("Text Editor");

    ZTabbedViewController* tabs = new ZTabbedViewController(argv, {textEditor, nodeView, view3d});
    ZApplication(tabs, "zpath", false, 1200, 800);
}
