
#include "zviewcontroller.h"

ZViewController::ZViewController(string resourcePath) {

	string vertexPath = resourcePath + "resources/shaders/uivertexshader.glsl";
    string fragmentPath = resourcePath + "resources/shaders/uifragmentshader.glsl";
    mUIShader = new ZShader(vertexPath, fragmentPath);
 
	float panelColor[4] = {0.1, 0.1, 0.1, 1.0};
    float backgroundColor[4] = {0, 0, 0, 0};

	mRootView = new ZView(1920, 1920, mUIShader);
    mRootView->setOffset(0,0);
    mRootView->setMargin(2,2,2,2);
    mRootView->setBackgroundColor(backgroundColor);
    
    ZView* navBar = new ZView(1920, 30, mUIShader);
    navBar->setMargin(0,0,0,0);
    navBar->setBackgroundColor(panelColor);

    ZTiledView* propertiesPanel = new ZTiledView(300, 1920, mUIShader);

    propertiesPanel->setMargin(2,32,2,2);
    propertiesPanel->setBackgroundColor(panelColor);
   
    mRootView->addSubView(navBar);
    mRootView->addSubView(propertiesPanel);
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

