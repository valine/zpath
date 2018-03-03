#ifndef ZSCENE_H
#define ZSCENE_H

#include <vector>
#include <string> 
#include "mesh/zobject.h"
#include "mesh/zmesh.h"
#include "zobjloader.h"
#include "zcamera.h"
using namespace std;


class ZScene {
	
public: 
    ZScene(string resourcePath);
	void addObject(ZObject *object);
	vector<ZObject*> getObjects();

	ZCamera* getCamera();

	void setExposure(float exposure);
	float getExposure();
private:
	vector<ZObject*> mObjects;
	ZCamera *mCamera;

	float mExposure = 1.0;
};

#endif