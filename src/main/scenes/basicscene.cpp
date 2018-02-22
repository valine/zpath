
#include "basicscene.h"

BasicScene::BasicScene(string resourcePath) 
: ZScene(resourcePath) {

	ZObjLoader loader = ZObjLoader("");
	
	// ZObject* object = loader.loadObject(resourcePath + "resources/meshes/cube");
	// addObject(object);

	ZObject* dbs2 = loader.loadObject(resourcePath + "resources/meshes/dbs");
	addObject(dbs2);
}
