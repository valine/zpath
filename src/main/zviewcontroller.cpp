
#include "zviewcontroller.h"

ZViewController::ZViewController(string resourcePath) {

	string vertexPath = resourcePath + "resources/shaders/uivertexshader.glsl";
    string fragmentPath = resourcePath + "resources/shaders/uifragmentshader.glsl";
    mUIShader = new ZShader(vertexPath, fragmentPath);
 
	float panelColor[4] = {0.1, 0.1, 0.1, 1.0};
    float backgroundColor[4] = {0, 0, 0, 0};
    float highlightColor[4] = {0.1, 0.2, 0.9, 1.0};

	mRootView = new ZView(1920, 1920);
    mRootView->setOffset(0,0);
    mRootView->setMargin(0,0,0,0);
    mRootView->setBackgroundColor(backgroundColor);
    mRootView->setShader(mUIShader);
    
    ZView* navBar = new ZView(1920, 30);
    navBar->setMargin(0,0,0,0);
    navBar->setBackgroundColor(panelColor);

    ZView* propertiesPanel = new ZView(200, 1080);

    propertiesPanel->setMargin(2, 30 + 2, 2,2);
    propertiesPanel->setOffset(0, 0);
    propertiesPanel->setBackgroundColor(panelColor);
    propertiesPanel->setGravity(ZView::topRight);
   
    mRootView->addSubView(navBar);
    mRootView->addSubView(propertiesPanel);

    for (int i = 0; i < 10; i++) {
        ZView* highlight = new ZView(30, 30);
        highlight->setMargin(10,10,10,10);
        highlight->setOffset(0, i * 100);

        float color[4] = {0.1f, 0.1, i * 0.1 + 0.1, 0};

        highlight->setBackgroundColor(color);
        highlight->setGravity(ZView::topLeft);
        propertiesPanel->addSubView(highlight);
    } 

    for (int i = 0; i < 10; i++) {
        ZView* menuItem = new ZView(100, 50);
        menuItem->setMargin(4,4,4,4);
        menuItem->setOffset(i * 104, 0);
        menuItem->setBackgroundColor(backgroundColor);
        menuItem->setGravity(ZView::topLeft);
        navBar->addSubView(menuItem);
    } 
}

ZView* ZViewController::getRootView() {
	return mRootView;
}

void ZViewController::onWindowChange(int width, int height) {

	mUIShader->use();

	mParentWidth = width;
	mParentHeight = height;

    mRootView->onWindowChange(width, height);
}

void ZViewController::draw() {

	mRootView->draw();
	GLint vp_location = glGetUniformLocation(mUIShader->mID, "uVPMatrix");

	mat4 matrix;
    matrix = glm::rotate(matrix, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    mat4 projection = ortho(0.0f, (float) mParentWidth, (float) mParentHeight, 0.0f, -10.0f, 100.0f);
    glUniformMatrix4fv(vp_location, 1, GL_FALSE, glm::value_ptr(projection));
}

