
#include "zviewcontroller.h"

ZViewController::ZViewController(string resourcePath) {

	string vertexPath = resourcePath + "resources/shaders/uivertexshader.glsl";
    string fragmentPath = resourcePath + "resources/shaders/uifragmentshader.glsl";
    mUIShader = new ZShader(vertexPath, fragmentPath);
 
	float panelColor[4] = {0.1, 0.1, 0.1, 1.0};
    float backgroundColor[4] = {0, 0, 0, 0};
    float highlightColor[4] = {1, 0.1, 0.2, 1.0};

	mRootView = new ZView(1920, 1920, mUIShader);
    mRootView->setOffset(0,0);
    mRootView->setMargin(0,0,0,0);
    mRootView->setBackgroundColor(backgroundColor);
    
    ZView* navBar = new ZView(1920, 30, mUIShader);
    navBar->setMargin(0,0,0,0);
    navBar->setBackgroundColor(panelColor);

    ZTiledView* propertiesPanel = new ZTiledView(300, 600, mUIShader);

    propertiesPanel->setMargin(10,40,10,10);
    propertiesPanel->setOffset(0, 0);
    propertiesPanel->setBackgroundColor(panelColor);
    propertiesPanel->setGravity(ZView::topLeft);
   
    mRootView->addSubView(navBar);
    mRootView->addSubView(propertiesPanel);

    for (int i = 0; i < 5; i++) {
        ZView* highlight = new ZView(1000, 30, mUIShader);
        highlight->setMargin(2,2,2,2);
        highlight->setOffset(0, i * 31);
        highlight->setBackgroundColor(highlightColor);
        propertiesPanel->addSubView(highlight);
    }
   
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

