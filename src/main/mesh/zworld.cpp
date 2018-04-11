#include "zworld.h"

void ZWorld::setEnvironmentTexture(ZTexture* texture) {
	mEnvironmentTexture = texture;
}

ZTexture* ZWorld::getEnvironmentTexture() {
	return mEnvironmentTexture;
}

void ZWorld::blurBackground(bool shouldBlur) {
	mBlurredBackground = shouldBlur;
}

bool ZWorld::isBackgroundBlurred() {
	return mBlurredBackground;
}
