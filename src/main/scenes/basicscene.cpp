
#include "basicscene.h"

BasicScene::BasicScene(string resourcePath) 
: ZScene(resourcePath) {

	ZTexture* environmentTexture = new ZTexture(resourcePath + "resources/textures/light-blue.hdr");
	ZWorld* world = new ZWorld();
	world->setEnvironmentTexture(environmentTexture);
	setWorld(world);

	ZObjLoader loader = ZObjLoader();

 	vector<ZObject*> objects = loader.loadObjects(resourcePath + "resources/meshes/brain.obj");
 	if (objects.size() > 0) {
	    for (unsigned i = 0; i < objects.size(); ++i) {
	        ZObject* object = objects.at(i);

	        addObject(object);
	        ZMaterial* brainMaterial = new ZMaterial(vec3(0.9, 0.1, 0.1));
	        brainMaterial->setRoughness(0.1);
	        brainMaterial->setMetallic(0);
	        object->setMaterial(brainMaterial);

	    }
	}

	ZObject* electrodes = loader.loadObject(resourcePath + "resources/meshes/dbs-electrodes.obj");
	
	ZMaterial* electrodeMaterial = new ZMaterial(vec3(0.1, 0.1, 0.1));
	electrodeMaterial->setMetallic(1.0);
	electrodeMaterial->setRoughness(0.2);
	electrodes->setMaterial(electrodeMaterial);

	ZObject* lead = loader.loadObject(resourcePath + "resources/meshes/dbs-lead.obj");
	ZMaterial* leadMaterial = new ZMaterial(vec4(0.6, 0.6, 0.6, 0.8));
	leadMaterial->setMetallic(0);
	leadMaterial->setRoughness(0.1);
	lead->setMaterial(leadMaterial);


	// ZObject* brainTransparent = loader.loadObjects(resourcePath + "resources/meshes/brain-transparent.obj").at(0);
	// ZMaterial* transparentMaterial = new ZMaterial(vec4(0.01, 0.02, 0.06, 0.2));
	// transparentMaterial->setMetallic(0);
	// transparentMaterial->setRoughness(0.1);
	// brainTransparent->setMaterial(transparentMaterial);


	// Add objects 
	
	//addObject(electrodes);
	//addObject(brain);
	//addObject(lead);
	//addObject(brainTransparent);


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
