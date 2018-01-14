
#include "zviewcontroller.h"

ZViewController::ZViewController(string resourcePath) {

	string vertexPath = resourcePath + "resources/shaders/uivertexshader.glsl";
    string fragmentPath = resourcePath + "resources/shaders/uifragmentshader.glsl";

    mUIShader = new ZShader(vertexPath, fragmentPath);
 
	mRootView = new ZView(300, 500, mUIShader);
    mRootView->setMargin(10,10,10,10);
}

void ZViewController::addSubView(ZView *view) {
	mViews.push_back(view);
}

ZView* ZViewController::getRootView() {
	return mRootView;
}

void ZViewController::onWindowChange(int width, int height) {

	mParentWidth = width;
	mParentHeight = height;

	mRootView->onWindowChange(width, height);
}

void ZViewController::draw() {

	mUIShader->use();

	GLint vp_location = glGetUniformLocation(mUIShader->mID, "uVPMatrix");

    mat4 matrix;
    matrix = glm::rotate(matrix, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    mat4 projection = ortho(0.0f, (float) mParentWidth, (float) mParentHeight, 0.0f, -10.0f, 100.0f);
    glUniformMatrix4fv(vp_location, 1, GL_FALSE, glm::value_ptr(projection));

	mRootView->draw();
}

