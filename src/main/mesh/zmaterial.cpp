#include "zmaterial.h"


ZMaterial::ZMaterial(vec3 color) {
	mColor = color;
}

vec3 ZMaterial::getColor() {
	return mColor;
}

void ZMaterial::setColor(vec3 color) {
	mColor = color;
}

ZTexture* ZMaterial::getColorTexture() {
	return mColorTexture;
}

void ZMaterial::setColorTexture(ZTexture* texture) {
	mColorTexture = texture;
}

float ZMaterial::getMetallic() {
	return mMetallic;
}

void ZMaterial::setMetallic(float factor) {
	mMetallic = factor;
}

float ZMaterial::getRoughness() {
	return mRoughness;
}

void ZMaterial::setRoughness(float factor) {
	mRoughness = factor;
}