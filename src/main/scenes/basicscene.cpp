
#include "scenes/basicscene.h"

BasicScene::BasicScene(string resourcePath) 
: ZScene(resourcePath) {

	ZTexture* environmentTexture = new ZTexture(resourcePath + "resources/textures/light-blue.hdr");
	ZWorld* world = new ZWorld();
	world->setColor(vec3(1,1,1));
	world->setEnvironmentTexture(environmentTexture);
	setWorld(world);

	//- Lighting setup ---------------------------------------------------------------
	setExposure(2.3);
	ZPointLight* light = new ZPointLight(vec3(3.47, 3.31, 3.79), vec3(0,0.9,1));
	addLight(light);
}
