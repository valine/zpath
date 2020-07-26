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


int main(int argc, char* argv[]) {

    vector<ZViewController*> controller = {new ZPathViewController(argv), new ZPathViewController(argv)};
    ZTabbedViewController* tabs = new ZTabbedViewController(argv, controller);
    ZApplication(tabs, "ZPATH", false);
}
