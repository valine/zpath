#include <utility>

#include "ui/zviewcontroller.h"


ZViewController::ZViewController(string resourcePath) : ZView(ZView::fillParent, ZView::fillParent) {
    mResourcePath = std::move(resourcePath);
    setDrawingEnabled(false);
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
    if (mDrawingEnabled) {
        ZView::onWindowChange(width, height);
        mParentWidth = width;
        mParentHeight = height;

        setWindowWidth(width);
        setWindowHeight(height);
    } else {
        ZView::onWindowChange(width, height);
    }
}

void ZViewController::onKeyPress(int key, int scancode, int action, int mods) {
    if (getVisibility()) {
        ZView::onKeyPress(key, scancode, action, mods);
    }
}
	
void ZViewController::onMouseEvent(int button, int action, int mods, int x, int y) {
    if (getVisibility()) {
        ZView::onMouseEvent(button, action, mods, x, y);
    }
}

void ZViewController::onCursorPosChange(double x, double y) {
    if (getVisibility()) {
        ZView::onCursorPosChange(x, y);
    }
}

void ZViewController::onScrollChange(double x, double y) {
    if (getVisibility()) {
        ZView::onScrollChange(x, y);
    }
}

void ZViewController::onTerminate() {
    onExit();
}

void ZViewController::draw() {
    if (mDrawingEnabled && getVisibility()) {
        mUIShader->use();
        GLint vp_location = glGetUniformLocation(mUIShader->mID, "uVPMatrix");
        mat4 projection = ortho(0.0f, (float) mParentWidth, (float) mParentHeight, 0.0f, -10.0f, 100.0f);
        glUniformMatrix4fv(vp_location, 1, GL_FALSE, glm::value_ptr(projection));

        mImageViewShader->use();
        GLint vp_locationi = glGetUniformLocation(mImageViewShader->mID, "uVPMatrix");
        glUniformMatrix4fv(vp_locationi, 1, GL_FALSE, glm::value_ptr(projection));

        glViewport(0, 0, mParentWidth, mParentHeight);
        ZView::draw();
    } else if (getVisibility()){
        ZView::draw();
    }
}

string ZViewController::getResourcePath() {
    return mResourcePath;
}

void ZViewController::setName(string name) {
    mName = name;
}

string ZViewController::getName() {
    return mName;
}
