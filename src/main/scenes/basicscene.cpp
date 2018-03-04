
#include "basicscene.h"

BasicScene::BasicScene(string resourcePath) 
: ZScene(resourcePath) {

	ZObjLoader loader = ZObjLoader("");
	
	// ZObject* object = loader.loadObject(resourcePath + "resources/meshes/cube");
	// addObject(object);

	ZObject* dbs2 = loader.loadObject(resourcePath + "resources/meshes/dbs2");
	addObject(dbs2);
	dbs2->setColor(vec3(0.058417, 0.326373, 1.000000));

	ZObject* dbs = loader.loadObject(resourcePath + "resources/meshes/dbs");
	addObject(dbs);
	dbs->setColor(vec3(1.0, 0.9, 0.9));
}
