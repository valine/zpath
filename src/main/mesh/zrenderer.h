#ifndef ZRENDERER_H
#define ZRENDERER_H

#include <string>
#include "zscene.h"
#include "ui/zshader.h"
#include "ui/z3dview.h"
#include "zcamera.h"
#include "zpointlight.h"
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

using std::string;

class Z3DView; 

class ZRenderer {

public:
	ZRenderer(string debug);
	void setScene(ZScene *scene);
	void draw();
	void setParentView(Z3DView *view);	
	void init();
	virtual void onDrawFinshed();

	void renderCube();

	ZCamera* getCamera();

private:
	ZScene *mScene = nullptr;
	ZShader *mShader;
	ZShader *mColorTextureShader;

	ZShader *mHDRShader = nullptr;
	ZShader *mBackgroundShader = nullptr;
	ZShader *mCubemapShader = nullptr;
	ZShader *mIrradienceShader = nullptr;
	ZShader *mPrefilterShader = nullptr;
	ZShader *mBrdfShader = nullptr;

	Z3DView *mParentView = nullptr;

	ZCamera* mCamera;

	unsigned int mHdrFBO;
	unsigned int mColorBuffer;
	unsigned int mRenderBuffer;

	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;

	unsigned int envCubemap;
	unsigned int irradienceCubemap;

	unsigned int brdfLUTTexture;


	unsigned int prefilterMap;

	void renderQuad();

	int mMaxLightCount = 4;
};

#endif
