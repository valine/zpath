#include <utility>
#include <utils/zsettingsstore.h>

#include "ui/zviewcontroller.h"


ZViewController::ZViewController(string resourcePath) : ZView(ZView::fillParent, ZView::fillParent) {
    mResourcePath = std::move(resourcePath);
    setBackgroundColor(ZSettingsStore::get().getBackgroundColor());
    setDrawingEnabled(false);
    setRootView(this);
}


ZViewController::ZViewController(const char **argv) : ZView(ZView::fillParent, ZView::fillParent) {

    std::string aux(argv[0]);

    // get '/' or '\\' depending on unix/mac or windows.
#if defined(_WIN32) || defined(WIN32)
    int pos = aux.rfind('\\');
#else
    int pos = aux.rfind('/');
#endif


    // Get the path and the name
    std::string path = aux.substr(0,pos+1);
    std::string name = aux.substr(pos+1);
    mResourcePath = path;
    setRootView(this);
}

ZViewController::ZViewController(char **argv) : ZView(ZView::fillParent, ZView::fillParent) {
    std::string aux(argv[0]);

    // get '/' or '\\' depending on unix/mac or windows.
#if defined(_WIN32) || defined(WIN32)
    int pos = aux.rfind('\\');
#else
    int pos = aux.rfind('/');
#endif


    // Get the path and the name
    std::string path = aux.substr(0,pos+1);
    std::string name = aux.substr(pos+1);
    mResourcePath = path;
    setRootView(this);

    setBackgroundColor(white);
}


void ZViewController::onCreate() {
    if (mDrawingEnabled) {
        mUIShader = new ZShader(ui_vs, ui_fs);
        mTextShader = new ZShader(text_vs, text_fs);
        mImageViewShader = new ZShader(ui_vs, image_fs);

        ZView::onCreate();
        setShader(mUIShader);
        setTextShader(mTextShader);
        setImageShader(mImageViewShader);
    } else {
        ZView::onCreate();
    }

}

void ZViewController::setDrawingEnabled(bool draw) {
    mDrawingEnabled = draw;
}

void ZViewController::onFileDrop(int count, const char** paths) {
    ZView::onFileDrop(count, paths);
}

void ZViewController::onWindowChange(int width, int height) {

    calculateLeft();
    calculateTop();
    calculateBottom();
    calculateRight();

    mParentWidth = width;
    mParentHeight = height;

    if (mDrawingEnabled) {

        setWindowWidth(width);
        setWindowHeight(height);

        ZView::onWindowChange(width, height);
    } else {

        ZView::onWindowChange(width, height);
    }
}


void ZViewController::onMouseEvent(int button, int action, int mods, int x, int y) {
    if (getVisibility()) {
        ZView::onMouseEvent(button, action, mods, x, y);
    }

    if (action == GLFW_RELEASE) {
        for (int i = 0; i < getSubViews().size(); i++) {
            if (getSubViews().size() > i) {
                getSubViews().at(i)->onGlobalMouseUp(button);
            }
        }
    }
}

void ZViewController::onCursorPosChange(double x, double y) {
    if (getVisibility()) {
        ZView::onCursorPosChange(x, y);
    }
}

void ZViewController::onScrollChange(double x, double y) {
    if (getVisibility()) {
        if (isViewInFocus()) {
            mFocusedView->onScrollChange(x,y);
        } else {
            ZView::onScrollChange(x, y);
        }
    }
}

void ZViewController::onTerminate() {
    onExit();
}

void ZViewController::draw() {

    if (mDrawingEnabled) {

        mUIShader->use();

        GLint vp_location = glGetUniformLocation(mUIShader->mID, "uVPMatrix");
        mat4 projection = ortho(0.0f, (float) mParentWidth, (float) mParentHeight, 0.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(vp_location, 1, GL_FALSE, glm::value_ptr(projection));

        mImageViewShader->use();

        GLint vp_locationi = glGetUniformLocation(mImageViewShader->mID, "uVPMatrix");
        glUniformMatrix4fv(vp_locationi, 1, GL_FALSE, glm::value_ptr(projection));
        ZView::draw();
    } else if (getVisibility()){
        ZView::draw();
    }
}

string ZViewController::getResourcePath() {
    return mResourcePath;
}

void ZViewController::requestFocus(ZView *view) {
    mFocusedView = view;
    onFocusChanged(mFocusedView);
}

bool ZViewController::isViewInFocus() {
    return mFocusedView != nullptr;
}

void ZViewController::releaseFocus(ZView *forView) {
    if (mFocusedView == forView) {
        mFocusedView = nullptr;
    }
}

void ZViewController::onKeyPress(int key, int scancode, int action, int mods) {
    if (isViewInFocus()) {
        mFocusedView->onKeyPress(key, scancode, action, mods);
    } else {
        if (getVisibility()) {
            ZView::onKeyPress(key, scancode, action, mods);
        }
    }
}

void ZViewController::onCharacterInput(unsigned int character) {
    if (isViewInFocus()) {
        mFocusedView->onCharacterInput(character);
    } else {
        ZView::onCharacterInput(character);
    }

}
