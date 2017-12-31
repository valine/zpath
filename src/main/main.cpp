#include "main.h"

#include <iostream>
using std::cout;
using std::endl;
#include <fstream>
#include <string>

// when testing, the main of gtest and this main function will collide,
// this prevents the collision, but it's an ugly hack like all ifdef branches
#ifdef UNIT_TESTS
#define MAIN not_main
#else
#define MAIN main
#endif

int MAIN(int argc, char** argv) {


  for (int i = 0; i < 101; i++) {

    cout << i;
  }
  return EXIT_SUCCESS;
}


// just some function to have something that can be test
#include <exception>

void GetFrobnicationInterval() {
  throw std::string("InvalidOperation: frobnication interval can't be retrieved");
}

