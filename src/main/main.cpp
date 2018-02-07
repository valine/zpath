#include "main.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#include <fstream>
#include <string>

#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <ui/zapplication.h>

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}


int main(int argc, char* argv[]) {
    signal(SIGSEGV, handler);
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
