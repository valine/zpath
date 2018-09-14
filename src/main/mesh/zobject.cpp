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

void ZObject::translateBy(vec3 translate) {
	mTranslation = mTranslation + translate;
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

void ZObject::rotateBy(float angle) {
	mRotationAngle += angle;
}

void ZObject::setScale(vec3 scale) {
	mScale = scale;
}

vec3 ZObject::getScale() {
	return mScale;
}

void ZObject::scaleBy(vec3 scale) {
	vec3 tmpScale = mScale + scale;

	if (tmpScale.x > 0) {
		mScale = mScale + scale;
	}
}

void ZObject::setTransform(mat4 transform) {
	mTransform = transform;
}

mat4 ZObject::getTransform() {
	return mTransform;
}

void ZObject::transformBy(mat4 transform) {
	mTransform = transform * mTransform;
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

ZObject* ZObject::getParent() {
	return mParent;
}

void ZObject::setParent(ZObject* parent) {
	mParent = parent;
	// if (mParent != nullptr) {
	// 	parent->addChild(this);
	// }
}

void ZObject::addChild(ZObject* child) {
	mChildren.push_back(child);
}

vector<ZObject*> ZObject::getChildren() {
	return mChildren;
}

void ZObject::setSelectable(bool select) {
	mSelectable = select;
}

bool ZObject::getSelectable() {
	return mSelectable;
}

void ZObject::startAnimation(ZAnimator* animator) {
	cout<<mAnimators.size()<<endl;
	mAnimators.clear();
	mAnimators.push_back(animator);
}

vector<ZAnimator*> ZObject::getAnimators() {
	return mAnimators;
}

void ZObject::animationFinished(ZAnimator* animator) {
	mAnimators.erase(remove(mAnimators.begin(), mAnimators.end(), animator), mAnimators.end());
}




