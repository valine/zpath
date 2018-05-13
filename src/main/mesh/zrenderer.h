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
	ZScene* getScene();
	void setScene(ZScene *scene);
	void draw();
	void setParentView(Z3DView *view);	
	void init();
	virtual void onDrawFinshed();

	void renderCube();
	int getObjectIndexAtLocation(int x, int y);
	ZCamera* getCamera();

private:
	ZScene *mScene = nullptr;
	ZShader *mShader;
	ZShader *mColorTextureShader;

	ZShader *mHDRShader;
	ZShader *mBackgroundShader;
	ZShader *mSelectionShader;

	Z3DView *mParentView = nullptr;

	ZCamera* mCamera;

	unsigned int mMainFBO;
	unsigned int mMainBuffer;
	unsigned int mRenderBuffer;
	
	unsigned int mSelectionFBO;
	unsigned int mSelectionBuffer;
	unsigned int mSelectionRenderBuffer;

	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;

	void renderMain();
	void renderSelection();
	void renderToScreen();

	void renderQuad();

	int mMaxLightCount = 4;
};

#endif
