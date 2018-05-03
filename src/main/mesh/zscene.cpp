#include "zscene.h"

ZScene::ZScene(string resourcePath) {
	mCamera = new ZCamera();
}

void ZScene::addObject(ZObject* object) {
	mObjects.push_back(object);
}

void ZScene::addLight(ZPointLight* light) {
	mPointLights.push_back(light);
	updateLightFlatArrays();
}

vector<ZObject*> ZScene::getObjects() {
	return mObjects;
}

vector<ZPointLight*> ZScene::getLights() {
	return mPointLights;
}

float* ZScene::getLightPositions() {
	return mLightFlatPosition;
}

float* ZScene::getLightColors() {
	return mLightFlatColor;
}

ZCamera* ZScene::getCamera() {
	return mCamera;
}

void ZScene::setWorld(ZWorld* world) {
	mWorld = world;
}

ZWorld* ZScene::getWorld() {
	return mWorld;
}

void ZScene::setExposure(float exposure) {
	mExposure = exposure;
}

float ZScene::getExposure() {
	return mExposure;
}

void ZScene::setActiveObjectIndex(int index) {
	mActiveObjectIndex = index;
}

int ZScene::getActiveObjectIndex() {
	return mActiveObjectIndex;
}

void ZScene::updateLightFlatArrays() {
	mLightPositions.clear();
	mLightColors.clear();

	vector<ZPointLight*> lights = getLights();
	for (vector<ZPointLight*>::iterator it = mPointLights.begin() ; it != mPointLights.end(); ++it) {
		ZPointLight *light = (*it);
		mLightPositions.push_back(light->getPosition());
		mLightColors.push_back(light->getColor());
	}

	mLightFlatColor = static_cast<float*>(glm::value_ptr(mLightColors.front()));
	mLightFlatPosition = static_cast<float*>(glm::value_ptr(mLightPositions.front()));
}