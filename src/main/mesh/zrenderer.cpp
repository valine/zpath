#include "zrenderer.h"



ZRenderer::ZRenderer(string resourcePath) {

	string vertexPath = resourcePath + "resources/shaders/generalvertexshader.glsl";
    string fragmentPath = resourcePath + "resources/shaders/generalfragmentshader.glsl";
    mShader = new ZShader(vertexPath, fragmentPath);

	mPositionLocation = glGetAttribLocation(mShader->mID, "vPos");
	mColorLocation = glGetUniformLocation(mShader->mID, "uColor");

}

void ZRenderer::draw() {
	if (mScene != nullptr) {
		std::cout << "drawing";
	}
}

void ZRenderer::setScene(ZScene *scene) {
	mScene = scene;
}