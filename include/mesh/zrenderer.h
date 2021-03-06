#ifndef ZRENDERER_H
#define ZRENDERER_H

#include "glad/glad.h"
#include <string>
#include "zscene.h"
#include "ui/zshader.h"
#include "ui/z3dview.h"
#include "zcamera.h"
#include "zpointlight.h"
#include <iostream>
#include "zmeshutils.h"
#include "zrenderutils.h"
#include "zanimator.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace glm;

using std::string;

class ZView; 

class ZRenderer {

public:
	ZRenderer(string debug);
	ZScene* getScene();
	void setScene(ZScene *scene);
	void draw();
	void setParentView(ZView *view);	
	void init();
	virtual void onDrawFinshed();

	void renderCube();
	int getObjectIndexAtLocation(int x, int y);
	ZCamera* getCamera();
	void setCamera(ZCamera*);

	unsigned int getMainTexture();
	void setRenderToTexture(bool);
	void recreateBuffers();
	void onExit();
	int getMainBuffer();
	unsigned int getMainFBO();
	void setAASamples(int);

	void setWireMode(bool);

private:
	ZScene* mScene = nullptr;
	ZShader* mShader;
	ZShader* mColorTextureShader;

	ZShader* mHDRShader;
	ZShader* mBackgroundShader;
	ZShader* mSelectionShader;

	ZView* mParentView = nullptr;
	ZCamera* mCamera;
	ZMeshUtils meshUtils = ZMeshUtils();

    unsigned int mRenderBuffer;
    unsigned int mMainFBO;
    unsigned int mMainBuffer;
	
	unsigned int mFinalFBO;
	unsigned int mFinalBuffer;
	unsigned int mFinalRenderBuffer;

	unsigned int mSelectionFBO;
	unsigned int mSelectionBuffer;
	unsigned int mSelectionRenderBuffer;

	bool mWireMode = false;

	GLuint quadVAO = 0;
	GLuint quadVBO = 0;

	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;

	bool mRenderToTexture = false;
	int mSamples = 16;

	vector<ZObject*> mSortedObjects;
	vector<int> mSortedIndicies;

	void renderMain();
	void renderSelection();
	void renderToScreen();
	void renderQuad();
	void updateAnimations();
	void sortObjects();


	int mMaxLightCount = 4;

	// Shader code

	const string base_vs =
	 #include "shaders/base.vs"
	;

	const string base_fs =
	 #include "shaders/base.fs"
	;

	const string background_vs =
	 #include "shaders/background.vs"
	;

	const string background_fs =
	 #include "shaders/background.fs"
	;

	const string color_texture_fs =
	 #include "shaders/colorTexture.fs"
	;

	const string selection_vs =
	 #include "shaders/selection.vs"
	;

	const string selection_fs =
	 #include "shaders/selection.fs"
	;

	const string fbo_vs =
	 #include "shaders/fbo.vs"
	;

	const string fbo_fs =
	 #include "shaders/fbo.fs"
	;
};

#endif
