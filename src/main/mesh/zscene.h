#ifndef ZSCENE_H
#define ZSCENE_H

#include <vector>
#include <string> 
#include "mesh/zobject.h"
#include "mesh/zmesh.h"
#include "mesh/zpointlight.h"
#include "mesh/zworld.h"
#include "zobjloader.h"
#include "zcamera.h"
#include "zpath_constants.h"
using namespace std;


class ZScene {
	
public: 
    ZScene(string resourcePath);
	
	void addObject(ZObject* object);
	vector<ZObject*> getObjects();

	void addLight(ZPointLight* light);
	vector<ZPointLight*> getLights();

	float* getLightPositions();
	float* getLightColors();

	ZCamera* getCamera();

	void setWorld(ZWorld* world);
	ZWorld* getWorld();

	void setExposure(float exposure);
	float getExposure();

	void setActiveObjectIndex(int index);
	int getActiveObjectIndex();

	void deleteSelectedObject();
private:
	vector<ZObject*> mObjects;
	vector<ZPointLight*> mPointLights;

	vector<vec3> mLightPositions;
	vector<vec3> mLightColors;

	float* mLightFlatPosition;
	float* mLightFlatColor;

	ZCamera *mCamera;

	ZWorld* mWorld;

	float mExposure = 1.0;

	void updateLightFlatArrays();

	int mActiveObjectIndex = 0;
};

#endif