
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

	ZPointLight* light = new ZPointLight(vec3(1.1, 1.15, 1.5), vec3(3.0,3.0,3.0));
	addLight(light);

	ZPointLight* light2 = new ZPointLight(vec3(1, 1.1, 1.5), vec3(-10.0,-6.0,-10.0));
	addLight(light2);

	ZPointLight* light3 = new ZPointLight(vec3(1.0, 1.2, 1.6), vec3(-10.0,10.0,-10.0));
	addLight(light3);

	ZPointLight* light4 = new ZPointLight(vec3(1.0, 1.2, 1.6), vec3(0, 10, 20));
	addLight(light4);

}

