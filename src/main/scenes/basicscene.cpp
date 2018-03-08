
#include "basicscene.h"

BasicScene::BasicScene(string resourcePath) 
: ZScene(resourcePath) {

	ZObjLoader loader = ZObjLoader("");

	ZObject* dbs2 = loader.loadObject(resourcePath + "resources/meshes/dbs2");
	addObject(dbs2);

	ZMaterial* blueMaterial = new ZMaterial(vec3(0.058417, 0.326373, 1.000000));
	dbs2->setMaterial(blueMaterial);

	ZObject* dbs = loader.loadObject(resourcePath + "resources/meshes/dbs");
	addObject(dbs);
}
