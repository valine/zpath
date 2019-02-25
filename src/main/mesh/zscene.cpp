#include "mesh/zscene.h"

ZScene::ZScene(string resourcePath) {
	mCamera = new ZCamera();
	ZWorld* world = new ZWorld();
	setWorld(world);
}

void ZScene::addObject(ZObject* object) {
	if (object != nullptr) {

		if (object->getChildren().size() > 0) {
			for (uint i = 0; i < object->getChildren().size(); ++i) {
				addObject( object->getChildren().at(i));
			}
		}

		// todo: there should be a better way
		if (object->getMaterial()->getColor().a < 0.5) {
			mObjects.push_back(object);
		} else {
			mObjects.insert(mObjects.begin(), object);
		}
	}
}

void ZScene::addLight(ZPointLight* light) {
	mPointLights.push_back(light);
	updateLightFlatArrays();
}

vector<ZObject*> ZScene::getObjects() {
	return mObjects;
}

vector<ZPointLight*> ZScene::getLights() {
	return mPointLights;
}

float* ZScene::getLightPositions() {
	return mLightFlatPosition;
}

float* ZScene::getLightColors() {
	return mLightFlatColor;
}

ZCamera* ZScene::getCamera() {
	return mCamera;
}

void ZScene::setWorld(ZWorld* world) {
	mWorld = world;
}

ZWorld* ZScene::getWorld() {
	return mWorld;
}

void ZScene::setExposure(float exposure) {
	mExposure = exposure;
}

float ZScene::getExposure() {
	return mExposure;
}

void ZScene::setActiveObjectIndex(int index) {
	mActiveObjectIndex = index;

	if (index != -1 && index < mObjects.size()) {
		if (mObjects.at(index) != nullptr && mObjects.at(index)->getVisible()) {
			mObjects.at(index)->onSelection();
		}
	}
}

int ZScene::getActiveObjectIndex() {
	return mActiveObjectIndex;
}

void ZScene::deleteSelectedObject() {
	mObjects.erase(mObjects.begin() + mActiveObjectIndex);
	mActiveObjectIndex = -1;
}

void ZScene::updateLightFlatArrays() {
	mLightPositions.clear();
	mLightColors.clear();

	vector<ZPointLight*> lights = getLights();
	for (vector<ZPointLight*>::iterator it = mPointLights.begin() ; it != mPointLights.end(); ++it) {
		ZPointLight *light = (*it);
		mat4 modelMat = ZRenderUtils::getModelMatrix(light, nullptr);
		mLightPositions.push_back(modelMat * vec4(light->getPosition(), 1.0));
		mLightColors.push_back(light->getColor() * pow(ZRenderUtils::extractScale(modelMat), vec3(2.0)));
	}

	mLightFlatColor = static_cast<float*>(glm::value_ptr(mLightColors.front()));
	mLightFlatPosition = static_cast<float*>(glm::value_ptr(mLightPositions.front()));
}

void ZScene::moveObjectToBack(int itemIndex) {
	auto it = mObjects.begin() + itemIndex;
    std::rotate(it, it + 1, mObjects.end());
}