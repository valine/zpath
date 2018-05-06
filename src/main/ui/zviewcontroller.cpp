#include "zviewcontroller.h"

void ZViewController::onClick(ZButton* sender) {

}

ZViewController::ZViewController(string resourcePath) {
    mResourcePath = resourcePath;
}

void ZViewController::onCreate() {
    string vertexPath = mResourcePath + "resources/shaders/ui.vs";
    string fragmentPath = mResourcePath + "resources/shaders/ui.fs";
    mUIShader = new ZShader(vertexPath, fragmentPath);

    string vertexTextPath = mResourcePath + "resources/shaders/text.vs";
    string fragmentTextPath = mResourcePath + "resources/shaders/text.fs";
    mTextShader = new ZShader(vertexTextPath, fragmentTextPath);

    float backgroundColor[4] = {0.4f, 0.4, 0.4, 1.000};

    mRootView = new ZView(ZView::fillParent, ZView::fillParent);
    mRootView->setOffset(0,0);
    mRootView->setMargin(0,0,0,0);
    mRootView->setBackgroundColor(backgroundColor);
    mRootView->setShader(mUIShader);
    mRootView->setTextShader(mTextShader);
}

ZView* ZViewController::getRootView() {
	return mRootView;
}

void ZViewController::onFileDrop(int count, const char** paths) {

}

void ZViewController::onSliderValueChanged(ZSlider* sender, float value) {
    
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

string ZViewController::getResourcePath() {
    return mResourcePath;
}
