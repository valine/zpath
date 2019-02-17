#include "ui/zviewcontroller.h"

void ZViewController::onClick(ZButton* sender) {

}

ZViewController::ZViewController(string resourcePath) {
    mResourcePath = resourcePath;
}

void ZViewController::onCreate() {
    mUIShader = new ZShader(ui_vs, ui_fs);
    mTextShader = new ZShader(text_vs, text_fs);
    mImageViewShader = new ZShader(ui_vs, image_fs);

    float backgroundColor[4] = {0.4f, 0.4, 0.4, 1.000};

    mRootView = new ZView(ZView::fillParent, ZView::fillParent);
    mRootView->setOffset(0,0);
    mRootView->setMargin(0,0,0,0);
    mRootView->setBackgroundColor(backgroundColor);
    mRootView->setShader(mUIShader);
    mRootView->setTextShader(mTextShader);
    mRootView->setImageShader(mImageViewShader);
}

ZView* ZViewController::getRootView() {
	return mRootView;
}

void ZViewController::setDrawingEnabled(bool draw) {
    mDrawingEnabled = draw;
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
}

void ZViewController::onScrollChange(double x, double y) {
    mRootView->onScrollChange(x, y);
}

void ZViewController::onTerminate() {
    mRootView->onExit();
}

void ZViewController::draw() {
    if (mDrawingEnabled) {
        mUIShader->use();
        GLint vp_location = glGetUniformLocation(mUIShader->mID, "uVPMatrix");
        mat4 matrix;
        matrix = glm::rotate(matrix, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        mat4 projection = ortho(0.0f, (float) mParentWidth, (float) mParentHeight, 0.0f, -10.0f, 100.0f);
        glUniformMatrix4fv(vp_location, 1, GL_FALSE, glm::value_ptr(projection));

        mImageViewShader->use();
        GLint vp_locationi = glGetUniformLocation(mImageViewShader->mID, "uVPMatrix");
        glUniformMatrix4fv(vp_locationi, 1, GL_FALSE, glm::value_ptr(projection));

        if (mRootView != nullptr) {
            glViewport(0, 0, mParentWidth, mParentHeight);
            mRootView->draw();
        }
    }
}

string ZViewController::getResourcePath() {
    return mResourcePath;
}
