
#include "zviewcontroller.h"

void ZViewController::onClick(ZButton* sender) {

    if (sender == mGridViewButton) {
        if (mIsQuadView) {
            mIsQuadView = false;
            mTileView->setTileCount(1,1);
        } else {
            mIsQuadView = true;
            mTileView->setTileCount(3,2);
        }
    } else if (sender == mIncrementButton) {
        ZScene* scene = mTileView->getScene();
        float currentExposure = scene->getExposure();
        scene->setExposure(currentExposure + 0.1f);
    } else if (sender == mDecrementButton) {
        ZScene* scene = mTileView->getScene();
        float currentExposure = scene->getExposure();
        scene->setExposure(currentExposure - 0.1f);
    }
}

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


    ZView* propertiesPanel = new ZView(300, ZView::fillParent);
    propertiesPanel->setOffset(0, 22);
    propertiesPanel->setBackgroundColor(panelColor);
    propertiesPanel->setGravity(ZView::topRight);
    mRootView->addSubView(propertiesPanel);

    ZLabel* label = new ZLabel(100, 21, "roboto/Roboto-Bold.ttf", resourcePath);
    label->setOffset(10,0);
    label->setText("Properties");
    label->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(label);

    mGridViewButton = new ZButton(200, 40, resourcePath);
    mGridViewButton->setOffset(0,50);
    mGridViewButton->setMargin(10,10,10,10);
    mGridViewButton->setBackgroundColor(highlightColor);
    mGridViewButton->setText("Toggle Grid View");
    mGridViewButton->setOnClickListener(this);
    propertiesPanel->addSubView(mGridViewButton);

    mIncrementButton = new ZButton(40, 40, resourcePath);
    mIncrementButton->setOffset(0,100);
    mIncrementButton->setMargin(10,10,10,10);
    mIncrementButton->setBackgroundColor(highlightColor);
    mIncrementButton->setText("+");
    mIncrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mIncrementButton);

    mDecrementButton = new ZButton(40, 40, resourcePath);
    mDecrementButton->setOffset(50,100);
    mDecrementButton->setMargin(10,10,10,10);
    mDecrementButton->setBackgroundColor(highlightColor);
    mDecrementButton->setText("-");
    mDecrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mDecrementButton);

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

    int viewportWidth = mRootView->getWidth() - propertiesPanel->getWidth();
    int viewportHeight = 1000;
    mTileView = new ZTiledView(10000, 10000, 3, 2, resourcePath);
    mTileView->setOffset(propertiesPanel->getWidth(), 22);
    mTileView->setGravity(ZView::topRight);
    mRootView->addSubView(mTileView);
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

    if (mRootView != nullptr) {
	   mRootView->draw();
    }
}
