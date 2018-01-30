
#include "basicscene.h"

BasicScene::BasicScene(string resourcePath) 
: ZScene(resourcePath) {

	ZObjLoader loader = ZObjLoader("");
	ZObject* object = loader.loadObject(resourcePath + "resources/meshes/cube");
	addObject(object);

	ZObject* monkey = loader.loadObject(resourcePath + "resources/meshes/monkey");
	addObject(monkey);
}
