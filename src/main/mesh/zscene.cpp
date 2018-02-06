#include "zscene.h"

ZScene::ZScene(string resourcePath) {
	mCamera = new ZCamera();
}

void ZScene::addObject(ZObject* object) {
	mObjects.push_back(object);
}

vector<ZObject*> ZScene::getObjects() {
	return mObjects;
}

ZCamera* ZScene::getCamera() {
	return mCamera;
}