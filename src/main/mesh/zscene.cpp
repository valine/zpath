#include "zscene.h"

ZScene::ZScene() {


}

void ZScene::addObject(ZObject* object) {

	mObjects.push_back(object);
}