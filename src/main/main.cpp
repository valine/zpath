#include "main.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#include <fstream>
#include <string>

#include <zrenderer.h>
#include <zapplication.h>


int main(int argc, char* argv[]) {

      // Get the last position of '/'
    std::string aux(argv[0]);

        // get '/' or '\\' depending on unix/mac or windows.
    #if defined(_WIN32) || defined(WIN32)
        int pos = aux.rfind('\\');
    #else
        int pos = aux.rfind('/');
    #endif

    // Get the path and the name
    std::string path = aux.substr(0,pos+1);
    std::string name = aux.substr(pos+1);
    // show results
    std::cout << "Path: " << path << std::endl;
    std::cout << "Name: " << name << std::endl;


    ZApplication application(path);
}
