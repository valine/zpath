#ifndef ZWORLD_H
#define ZWORLD_H

#include "ui/ztexture.h"
#include "ui/zshader.h"
#include <string>
#include <iostream>
#include <thread>

using namespace std;

class ZWorld {
	public:
		ZWorld(string resourcePath);
		void setEnvironmentTexture(ZTexture* texture);
		ZTexture* getEnvironmentTexture();
		void blurBackground(bool shouldBlur);
		bool isBackgroundBlurred();

		unsigned int getBackgroundID();
		unsigned int getIrradienceID();
		unsigned int getPrefilteredID();
		unsigned int getBrdfLutID();
	private:

		void initEnvironment();

		ZShader *mCubemapShader;
		ZShader *mIrradienceShader;
		ZShader *mPrefilterShader;
		ZShader *mBrdfShader;

		ZTexture* mEnvironmentTexture;
		bool mBlurredBackground = true;

		unsigned int quadVAO = 0;
		unsigned int quadVBO;

		unsigned int cubeVAO = 0;
		unsigned int cubeVBO = 0;

		unsigned int envCubemap;
		unsigned int irradienceCubemap;

		unsigned int brdfLUTTexture;
		unsigned int prefilterMap;

		void renderQuad();
		void renderCube();
};

#endif