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