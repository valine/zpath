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


int main(int argc, char* argv[]) {

   // vector<ZViewController*> controller = {new ZPathViewController(argv), new ZPathViewController(argv)};

    auto* view3d = new ZPathViewController(argv);
    view3d->setName("3D Viewport");
    auto* nodeView = new NodeViewController(argv);
    nodeView->setName("Node Editor");

    ZTabbedViewController* tabs = new ZTabbedViewController(argv, {nodeView, view3d});
    ZApplication(tabs, "ZPATH", false, 1200, 800);
}
