#ifndef ZWORLD_H
#define ZWORLD_H
#include "glad/glad.h"
#include "ui/ztexture.h"
#include "ui/zshader.h"
#include <string>
#include <iostream>
#include <thread>

using namespace std;

class ZWorld {
	public:
		ZWorld();
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

		// Shader code 
		const string cubemap_vs =
		#include "shaders/cubemap.vs"
		;

		const string cubemap_fs =
		#include "shaders/cubemap.fs"
		;

		const string prefilter_fs =
		#include "shaders/prefilter.fs"
		;

		const string irradiance_fs =
		#include "shaders/irradiance.fs"
		;

		const string brdf_vs =
		#include "shaders/brdf.vs"
		;

		const string brdf_fs =
		#include "shaders/brdf.fs"
		;
};

#endif