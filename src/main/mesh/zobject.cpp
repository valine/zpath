#define GLEW_STATIC

#include "zobject.h"

ZObject::ZObject() {
	mColor = vec3(1.0,1.0,0.0);
}

void ZObject::setMesh(ZMesh* mesh) {
	mMesh = mesh;
}

ZMesh* ZObject::getMesh() {
	return mMesh;
}

void ZObject::setColor(vec3 color) {
	mColor = color;
}

vec3 ZObject::getColor() {
	return mColor;
}
