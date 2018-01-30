#ifndef ZSCENE_H
#define ZSCENE_H

#include <vector>
#include "mesh/zobject.h"
#include "mesh/zmesh.h"
#include "zobjloader.h"
using std::vector;

class ZScene {
	
public: 
    ZScene();
	void addObject(ZObject *object);
	vector<ZObject*> getObjects();
private:
	vector<ZObject*> mObjects;
};

#endif