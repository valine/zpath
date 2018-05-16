#define GLEW_STATIC
#include "mesh/zobject.h"

ZObject::ZObject() {
	mMaterial = new ZMaterial(vec3(0.8, 0.8, 0.8));
}

void ZObject::setMesh(ZMesh* mesh) {
	mMesh = mesh;
	mMaterial = new ZMaterial(vec3(0.8, 0.8, 0.8));
}

ZMesh* ZObject::getMesh() {
	return mMesh;
}

void ZObject::setMaterial(ZMaterial* material) {
	mMaterial = material;
}

bool ZObject::isSelected() {
	return mSelected;
}

void ZObject::setSelected(bool selected) {
	mSelected = selected;
}

ZMaterial* ZObject::getMaterial() {
	return mMaterial;
}

vec3 ZObject::getOrigin() {
	return mOrigin;
}

void ZObject::setOrigin(vec3 origin) {
	mOrigin = origin;
}

	