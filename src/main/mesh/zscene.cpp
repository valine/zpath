#include "zscene.h"

ZScene::ZScene() {
	ZObjLoader loader = ZObjLoader("");
	ZObject* object = loader.loadObject("");
	addObject(object);
}

void ZScene::addObject(ZObject* object) {
	mObjects.push_back(object);
}

vector<ZObject*> ZScene::getObjects() {
	return mObjects;
}