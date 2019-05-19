#ifndef ZRAYTRACER_H
#define ZRAYTRACER_H

using namespace std;

#include <cstdio> 
#include <cstdlib> 
#include <memory> 
#include <vector> 
#include <utility> 
#include <cstdint> 
#include <iostream> 
#include <fstream> 
#include <cmath> 
#include <limits> 
#include <random> 
#include "mesh/zscene.h"

class ZRayTracer {

public: 

	ZRayTracer();
	void setScene(ZScene*);
	private:

	ZScene* mScene;
};

#endif