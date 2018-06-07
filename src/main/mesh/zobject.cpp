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

void ZObject::setTranslation(vec3 translation) {
	mTranslation = translation;
}

vec3 ZObject::getTranslation() {
	return mTranslation;
}

void ZObject::setRotation(vec3 rotation) {
	mRotation = rotation;
}

vec3 ZObject::getRotation() {
	return mRotation;
}

void ZObject::setRotationAngle(float angle) {
	mRotationAngle = angle;
}

float ZObject::getRotationAngle() {
	return mRotationAngle;
}

void ZObject::setScale(vec3 scale) {
	mScale = scale;
}

vec3 ZObject::getScale() {
	return mScale;
}


bool ZObject::isBillboard() {
	return mIsBillboard;
}

void ZObject::setBillboard(bool billboard) {
	mIsBillboard = billboard;
}

void ZObject::setModelMatrix(mat4 matrix) {
	mModelMatrix = matrix;
	mModelMatrixSet = true;
}

mat4 ZObject::getModelMatrix() {
	return mModelMatrix;
}

bool ZObject::getModelMatrixSet() {
	return mModelMatrixSet;
}



