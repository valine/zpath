#define GLEW_STATIC

#include "zobject.h"

ZObject::ZObject() {

}

void ZObject::setMesh(ZMesh* mesh) {
	mMesh = mesh;
}

ZMesh* ZObject::getMesh() {
	return mMesh;
}
