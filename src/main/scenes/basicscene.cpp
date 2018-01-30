
#include "basicscene.h"

BasicScene::BasicScene() 
: ZScene() {

	ZObjLoader loader = ZObjLoader("");
	ZObject* object = loader.loadObject("");
	addObject(object);
}
