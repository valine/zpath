#include "zworld.h"

void ZWorld::setEnvironmentTexture(ZTexture* texture) {
	mEnvironmentTexture = texture;
}

ZTexture* ZWorld::getEnvironmentTexture() {
	return mEnvironmentTexture;
}