
#include "basicscene.h"

BasicScene::BasicScene(string resourcePath) 
: ZScene(resourcePath) {

	ZTexture* environmentTexture = new ZTexture(resourcePath + "resources/textures/QueenMary_Chimney_Ref.hdr");
	ZWorld* world = new ZWorld();
	world->setEnvironmentTexture(environmentTexture);
	setWorld(world);

	ZObjLoader loader = ZObjLoader();

	ZObject* electrodes = loader.loadObject(resourcePath + "resources/meshes/dbs-electrodes.obj");
	addObject(electrodes);
	ZMaterial* electrodeMaterial = new ZMaterial(vec3(0.1, 0.1, 0.1));
	electrodeMaterial->setMetallic(1.0);
	electrodeMaterial->setRoughness(0.2);
	electrodes->setMaterial(electrodeMaterial);

	ZObject* lead = loader.loadObjects(resourcePath + "resources/meshes/dbs-lead.obj").at(0);
	addObject(lead);
	ZMaterial* leadMaterial = new ZMaterial(vec4(0.6, 0.6, 0.6, 0.8));
	leadMaterial->setMetallic(0);
	leadMaterial->setRoughness(0.1);
	lead->setMaterial(leadMaterial);

	ZObject* brain = loader.loadObjects(resourcePath + "resources/meshes/brain-opaque.obj").at(0);
	addObject(brain);
	ZMaterial* brainMaterial = new ZMaterial(vec3(0.9, 0.1, 0.1));
	brainMaterial->setMetallic(0);
	brainMaterial->setRoughness(0.1);
	brain->setMaterial(brainMaterial);


	ZObject* brainTransparent = loader.loadObjects(resourcePath + "resources/meshes/brain-transparent.obj").at(0);
	addObject(brainTransparent);
	ZMaterial* transparentMaterial = new ZMaterial(vec4(0.01, 0.02, 0.06, 0.2));
	transparentMaterial->setMetallic(0);
	transparentMaterial->setRoughness(0.1);
	brainTransparent->setMaterial(transparentMaterial);

	//- Lighting setup ---------------------------------------------------------------

	setExposure(2.3);
	ZPointLight* light = new ZPointLight(vec3(3.47, 3.31, 3.79), vec3(0,0.9,1));
	addLight(light);

	// ZPointLight* light2 = new ZPointLight(vec3(10, 11.1, 12.5), vec3(-10.0,-6.0,-10.0));
	// addLight(light2);

	// ZPointLight* light3 = new ZPointLight(vec3(31.0, 32.2, 32.6), vec3(-10.0,10.0,-10.0));
	// addLight(light3);

	// ZPointLight* light4 = new ZPointLight(vec3(42.0,42.2, 42.6), vec3(0, 10, 20));
	//addLight(light4);
}
