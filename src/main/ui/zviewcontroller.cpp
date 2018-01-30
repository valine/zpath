
#include "zviewcontroller.h"

ZViewController::ZViewController(string resourcePath) {

	string vertexPath = resourcePath + "resources/shaders/uivertexshader.glsl";
    string fragmentPath = resourcePath + "resources/shaders/uifragmentshader.glsl";
    mUIShader = new ZShader(vertexPath, fragmentPath);
 
	float panelColor[4] = {0.1, 0.1, 0.1, 1.0};
    float backgroundColor[4] = {0, 0, 0, 0};
    float highlightColor[4] = {0.1, 0.2, 0.9, 1.0};

	mRootView = new ZView(ZView::fillParent, ZView::fillParent);
    mRootView->setOffset(0,0);
    mRootView->setMargin(0,0,0,0);
    mRootView->setBackgroundColor(backgroundColor);
    mRootView->setShader(mUIShader);
    
    ZView* navBar = new ZView(ZView::fillParent, 30);
    navBar->setMargin(0,0,0,0);
    navBar->setBackgroundColor(panelColor);

    ZView* propertiesPanel = new ZView(200, ZView::fillParent);

    propertiesPanel->setMargin(2, 2, 2, 2);
    propertiesPanel->setOffset(0, 30);
    propertiesPanel->setBackgroundColor(panelColor);
    propertiesPanel->setGravity(ZView::topRight);
   
    //mRootView->addSubView(navBar);
   // mRootView->addSubView(propertiesPanel);

    for (int i = 0; i < 40; i++) {
        ZView* highlight = new ZView(ZView::fillParent, 30);
        highlight->setMargin(0,0,0,0);
        highlight->setOffset(0, i * 30);

        float color[4] = {0.1f, 0.1, i * 0.02 + 0.2, 0};

        highlight->setBackgroundColor(color);
        highlight->setGravity(ZView::topLeft);
        propertiesPanel->addSubView(highlight);
    } 

    for (int i = 0; i < 10; i++) {
        ZView* menuItem = new ZView(100, 50);
        menuItem->setMargin(2,2,2,2);
        menuItem->setOffset(i * 102, 0);
        menuItem->setBackgroundColor(backgroundColor);
        menuItem->setGravity(ZView::topLeft);
        navBar->addSubView(menuItem);
    } 

    int viewportWidth = mRootView->getWidth() - propertiesPanel->getWidth();
    int viewportHeight = 1000;
    ZTiledView *tileView = new ZTiledView(10000, 10000, 2, 1);
    tileView->setOffset(propertiesPanel->getWidth(), 30);
    tileView->setGravity(ZView::topRight);
    
    mRootView->addSubView(tileView);

    BasicScene* scene = new BasicScene();
    ZRenderer *renderer = new ZRenderer(resourcePath);
    renderer->setScene(scene);

    Z3DView *viewport3D = new Z3DView(200, 300, renderer);
    viewport3D->setBackgroundColor(highlightColor);
    viewport3D->setGravity(ZView::topLeft);
    tileView->addSubView(viewport3D);
}

ZView* ZViewController::getRootView() {
	return mRootView;
}

void ZViewController::onWindowChange(int width, int height) {

	mParentWidth = width;
	mParentHeight = height;

    mRootView->onWindowChange(width, height);
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

void ZViewController::draw() {

 mUIShader->use();
    GLint vp_location = glGetUniformLocation(mUIShader->mID, "uVPMatrix");
    mat4 matrix;
    matrix = glm::rotate(matrix, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    mat4 projection = ortho(0.0f, (float) mParentWidth, (float) mParentHeight, 0.0f, -10.0f, 100.0f);
    glUniformMatrix4fv(vp_location, 1, GL_FALSE, glm::value_ptr(projection));

	mRootView->draw();
}

