#include "zpointlight.h"

ZPointLight::ZPointLight(vec3 color, vec3 position) {
	mColor = color;
	mPosition = position;
}

vec3 ZPointLight::getColor() {
	return mColor;
}

void ZPointLight::setColor(vec3 color) {
	mColor = color;
}

vec3 ZPointLight::getPosition() {
	return mPosition;
}

void ZPointLight::setPosition(vec3 position) {
	
	mPosition = position;
}
