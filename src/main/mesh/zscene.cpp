#include "zscene.h"

ZScene::ZScene(string resourcePath) {

}

void ZScene::addObject(ZObject* object) {
	mObjects.push_back(object);
}

vector<ZObject*> ZScene::getObjects() {
	return mObjects;
}