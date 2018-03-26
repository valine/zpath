
#include "basicscene.h"

BasicScene::BasicScene(string resourcePath) 
: ZScene(resourcePath) {

	ZObjLoader loader = ZObjLoader("");

	ZMaterial* brainMaterial = new ZMaterial(vec3(0.093402, 0.211725, 0.420024));
	brainMaterial->setRoughness(0.1);

	ZObject* dbs2 = loader.loadObject(resourcePath + "resources/meshes/dbs2.obj");
	addObject(dbs2);
	dbs2->setMaterial(brainMaterial);
	
	ZObject* dbs = loader.loadObject(resourcePath + "resources/meshes/dbs.obj");
	addObject(dbs);

	ZObject* electrodes = loader.loadObject(resourcePath + "resources/meshes/dbs-electrodes.obj");
	addObject(electrodes);
	ZMaterial* electrodeMaterial = new ZMaterial(vec3(0.191540, 0.204470, 0.266761));
	electrodeMaterial->setMetallic(0.9);
	electrodeMaterial->setRoughness(0.7);
	electrodes->setMaterial(electrodeMaterial);

	ZObject* lead = loader.loadObject(resourcePath + "resources/meshes/dbs-lead.obj");
	addObject(lead);
	ZMaterial* leadMaterial = new ZMaterial(vec3(0.9, 0.9, 1.0));
	leadMaterial->setMetallic(0);
	leadMaterial->setRoughness(0.3);
	lead->setMaterial(leadMaterial);

	setExposure(1.5);
	ZPointLight* light = new ZPointLight(vec3(3.47, 3.31, 3.79), vec3(0,0.9,1));
	addLight(light);

	ZPointLight* light2 = new ZPointLight(vec3(10, 11.1, 12.5), vec3(-10.0,-6.0,-10.0));
	addLight(light2);

	ZPointLight* light3 = new ZPointLight(vec3(31.0, 32.2, 32.6), vec3(-10.0,10.0,-10.0));
	addLight(light3);

	ZPointLight* light4 = new ZPointLight(vec3(42.0,42.2, 42.6), vec3(0, 10, 20));
	addLight(light4);

}

