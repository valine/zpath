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


int main(int argc, char* argv[]) {

    vector<ZViewController*> controller = {new ZPathViewController(argv), new ZPathViewController(argv)};
    ZApplication(controller, "ZPATH", false);
}
