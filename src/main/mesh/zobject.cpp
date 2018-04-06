#define GLEW_STATIC
#include "zobject.h"

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

ZMaterial* ZObject::getMaterial() {
	return mMaterial;
}
	