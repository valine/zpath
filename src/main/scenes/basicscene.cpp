
#include "basicscene.h"

BasicScene::BasicScene(string resourcePath) 
: ZScene(resourcePath) {

	ZTexture* environmentTexture = new ZTexture(resourcePath + "resources/textures/studio.hdr");
	ZWorld* world = new ZWorld();
	world->setEnvironmentTexture(environmentTexture);
	setWorld(world);

	ZTexture* testTexture = new ZTexture(resourcePath + "resources/textures/brain1-shadow-map.png");


	ZObjLoader loader = ZObjLoader();

	ZMaterial* brainMaterial = new ZMaterial(vec3(0.093402, 0.211725, 0.420024));
	brainMaterial->setRoughness(0.2);

	ZObject* dbs2 = loader.loadObject(resourcePath + "resources/meshes/dbs2.obj");
	addObject(dbs2);
	
	dbs2->setMaterial(brainMaterial);
	
	ZMaterial* brainMaterialLarge = new ZMaterial(vec4(0.5,0.8,1, 0.8));
	brainMaterialLarge->setRoughness(0.1);
	brainMaterialLarge->setMetallic(0);
	//brainMaterialLarge->setColorTexture(testTexture);

	ZObject* dbs = loader.loadObject(resourcePath + "resources/meshes/dbs.obj");
	dbs->setMaterial(brainMaterialLarge);
	addObject(dbs);

	ZObject* electrodes = loader.loadObject(resourcePath + "resources/meshes/dbs-electrodes.obj");
	addObject(electrodes);
	ZMaterial* electrodeMaterial = new ZMaterial(vec3(0.501540, 0.504470, 0.566761));
	electrodeMaterial->setMetallic(1.0);
	electrodeMaterial->setRoughness(0.2);
	electrodes->setMaterial(electrodeMaterial);

	ZObject* lead = loader.loadObjects(resourcePath + "resources/meshes/dbs-lead.obj").at(0);
	addObject(lead);
	ZMaterial* leadMaterial = new ZMaterial(vec3(0.9, 0.9, 1.0));
	leadMaterial->setMetallic(0);
	leadMaterial->setRoughness(0.3);
	//leadMaterial->setColorTexture(testTexture);
	lead->setMaterial(leadMaterial);

	setExposure(2.3);
	ZPointLight* light = new ZPointLight(vec3(3.47, 3.31, 3.79), vec3(0,0.9,1));
	addLight(light);

	ZPointLight* light2 = new ZPointLight(vec3(10, 11.1, 12.5), vec3(-10.0,-6.0,-10.0));
	addLight(light2);

	ZPointLight* light3 = new ZPointLight(vec3(31.0, 32.2, 32.6), vec3(-10.0,10.0,-10.0));
	addLight(light3);

	ZPointLight* light4 = new ZPointLight(vec3(42.0,42.2, 42.6), vec3(0, 10, 20));
	addLight(light4);
}
