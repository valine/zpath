#ifndef ZRENDERER_H
#define ZRENDERER_H

#include <string>
#include "zscene.h"
#include "ui/zshader.h"
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

using std::string;

class ZRenderer {

public:
	ZRenderer(string debug);
	void setScene(ZScene *scene);
	void draw();	

private:
	ZScene *mScene = nullptr;
	ZShader *mShader;

	int mColorLocation;
	int mPositionLocation;
	int mNormalLocation;

	int mViewMatrixLocation;
	int mProjectionMatrixLocation;
	int mModelMatrixLocation;
};

#endif
