
#include "zviewcontroller.h"

ZViewController::ZViewController(string resourcePath) {

	string vertexPath = resourcePath + "resources/shaders/uivertexshader.glsl";
    string fragmentPath = resourcePath + "resources/shaders/uifragmentshader.glsl";
    mUIShader = new ZShader(vertexPath, fragmentPath);

    string vertexTextPath = resourcePath + "resources/shaders/textvertexshader.glsl";
    string fragmentTextPath = resourcePath + "resources/shaders/textfragmentshader.glsl";
    mTextShader = new ZShader(vertexTextPath, fragmentTextPath);
 
	float panelColor[4] = {0.9, 0.9, 0.91, 1.0};
    float backgroundColor[4] = {0.4f, 0.4, 0.4, 1.000};
    float highlightColor[4] = {0.1, 0.2, 0.9, 1.0};

	mRootView = new ZView(ZView::fillParent, ZView::fillParent);
    mRootView->setOffset(0,0);
    mRootView->setMargin(0,0,0,0);
    mRootView->setBackgroundColor(backgroundColor);
    mRootView->setShader(mUIShader);
    mRootView->setTextShader(mTextShader);


    ZView* propertiesPanel = new ZView(200, ZView::fillParent);
    propertiesPanel->setOffset(0, 22);
    propertiesPanel->setBackgroundColor(panelColor);
    propertiesPanel->setGravity(ZView::topRight);
    mRootView->addSubView(propertiesPanel);

     ZLabel* label = new ZLabel(100, 21, "roboto/Roboto-Bold.ttf", resourcePath);
    //  float color[4] = {0.2f, 0.2, 0.2, 1.0};
    
    // label->setGravity(ZView::topLeft);
    label->setOffset(10,0);
    label->setText("Properties");
    label->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(label);
    //label->setMargin(100,4,4,4);
    
    // ZLabel* label2 = new ZLabel(1000, 21, "roboto/Roboto-Bold.ttf", resourcePath);
    // label2->setMargin(4,4,4,4);
    // label2->setGravity(ZView::topLeft);
    // label2->setText("Hello");
    // label2->setTextColor(vec3(0.8,0.8,0.8));
    // label2->setOffset(100, 0);
   // propertiesPanel->addSubView(label2);


    ZView* navBar = new ZView(ZView::fillParent, 24);
    navBar->setBackgroundColor(panelColor);
    navBar->setGravity(ZView::topRight);
    mRootView->addSubView(navBar);

    string menuItemNames[4] = {"File", "Edit", "View", "Render"};
    for (int i = 0; i < 4; i++) {
         ZLabel* menuItem = new ZLabel(48, 21, "roboto/Roboto-Regular.ttf", resourcePath);
         menuItem->setOffset(i * 50 + 10, 0);
         menuItem->setText(menuItemNames[i]);
         menuItem->setTextColor(vec3(0.1,0.1,0.1));
         menuItem->setGravity(ZView::topLeft);
         navBar->addSubView(menuItem);
    } 

    int viewportWidth = mRootView->getWidth() - propertiesPanel->getWidth() - 2;
    int viewportHeight = 1000;
    ZTiledView *tileView = new ZTiledView(10000, 10000, 3, 2, resourcePath);
    tileView->setOffset(propertiesPanel->getWidth() + 2, 22);
    tileView->setGravity(ZView::topRight);
    
    mRootView->addSubView(tileView);
}

ZView* ZViewController::getRootView() {
	return mRootView;
}

void ZViewController::onWindowChange(int width, int height) {

	mParentWidth = width;
	mParentHeight = height;

    mRootView->onWindowChange(width, height);
    mRootView->setWindowWidth(width);
    mRootView->setWindowHeight(height);
}

void ZViewController::onKeyPress(int key, int scancode, int action, int mods) {
	mRootView->onKeyPress(key, scancode, action, mods);
}
	
void ZViewController::onMouseEvent(int button, int action, int mods, int x, int y) {
	mRootView->onMouseEvent(button, action, mods, x, y);
}

void ZViewController::onCursorPosChange(double x, double y) {
	mRootView->onCursorPosChange(x, y);
	mRootView->onWindowChange(mParentWidth, mParentHeight);
}

void ZViewController::onScrollChange(double x, double y) {

    mRootView->onScrollChange(x, y);
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

