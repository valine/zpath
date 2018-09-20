
#include "scenes/basicscene.h"

BasicScene::BasicScene(string resourcePath) 
: ZScene(resourcePath) {

	ZTexture* environmentTexture = new ZTexture(resourcePath + "resources/textures/light-blue.hdr");
	ZWorld* world = new ZWorld();
	world->setColor(vec3(1,1,1));
	world->setEnvironmentTexture(environmentTexture);
	world->blurBackground(true);
	setWorld(world);

	//- Lighting setup ---------------------------------------------------------------
	setExposure(2.3);
	ZPointLight* light = new ZPointLight(vec3(3.47, 3.31, 3.79), vec3(0,0.9,1));
	addLight(light);

	ZObjLoader loader = ZObjLoader();
	vector<ZObject*> objects = loader.loadObjects(resourcePath + "resources/meshes/monkey.dae");

	int i = 0;
	for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
		ZObject *object = (*it);
		//if (i % 3 ==0) {
			//object->setBillboard(true);
		//}
		mat4 model = ZRenderUtils::getModelMatrix(object, nullptr);
		cout << "model matrix::" << ZRenderUtils::extractScale(model).x <<endl;
		cout << "translation::" << ZRenderUtils::extractTranslation(model).x << endl;

		addObject(object);
		i++;
	}
}
