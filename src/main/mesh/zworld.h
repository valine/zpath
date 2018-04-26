#ifndef ZWORLD_H
#define ZWORLD_H

#include "ui/ztexture.h"
#include <string>

using namespace std;

class ZWorld {
	public:
		void setEnvironmentTexture(ZTexture* texture);
		ZTexture* getEnvironmentTexture();
		void blurBackground(bool shouldBlur);
		bool isBackgroundBlurred();
	private:
		ZTexture* mEnvironmentTexture;
		bool mBlurredBackground = true;
};

#endif