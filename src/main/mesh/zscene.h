#ifndef ZSCENE_H
#define ZSCENE_H

#include <vector>
#include <string> 
#include "mesh/zobject.h"
#include "mesh/zmesh.h"
#include "zobjloader.h"
using namespace std;


class ZScene {
	
public: 
    ZScene(string resourcePath);
	void addObject(ZObject *object);
	vector<ZObject*> getObjects();
private:
	vector<ZObject*> mObjects;
};

#endif