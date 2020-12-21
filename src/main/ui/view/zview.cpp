#include <utils/zsettingsstore.h>
#include "ui/zview.h"

ZView::ZView(float maxWidth, float maxHeight) {
    setOutlineColor(ZSettingsStore::getInstance().getInactiveColor());
    mMaxWidth = maxWidth;
    mMaxHeight = maxHeight;
    mMaxWidth = maxWidth;
    mMaxHeight = maxHeight;
    mVertices[4] = (GLfloat) mMaxWidth;
    mVertices[8] = (GLfloat) mMaxHeight;
    mVertices[12] = (GLfloat) mMaxWidth;
    mVertices[13] = (GLfloat) mMaxHeight;
    mParentView = this;
}

ZView::ZView(float maxWidth, float maxHeight, ZView* parent) {
    setOutlineColor(ZSettingsStore::getInstance().getInactiveColor());
    mMaxWidth = maxWidth;
    mMaxHeight = maxHeight;
    mMaxWidth = maxWidth;
    mMaxHeight = maxHeight;
    mVertices[4] = (GLfloat) mMaxWidth;
    mVertices[8] = (GLfloat) mMaxHeight;
    mVertices[12] = (GLfloat) mMaxWidth;
    mVertices[13] = (GLfloat) mMaxHeight;
    mWindowHeight = parent->getWindowHeight();
    mWindowWidth = parent->getWindowWidth();
    mParentView = parent;
    parent->addSubView(this);
}

ZView::ZView(Bounds maxWidth, float maxHeight) {
    setOutlineColor(ZSettingsStore::getInstance().getInactiveColor());
    mMaxWidth = 10000;
    mMaxHeight = maxHeight;
    mMaxWidth = maxWidth;
    mMaxHeight = maxHeight;
    mVertices[4] = (GLfloat) mMaxWidth;
    mVertices[8] = (GLfloat) mMaxHeight;
    mVertices[12] = (GLfloat) mMaxWidth;
    mVertices[13] = (GLfloat) mMaxHeight;
    mParentView = this;
}

ZView::ZView(float maxWidth, Bounds maxHeight) {
    setOutlineColor(ZSettingsStore::getInstance().getInactiveColor());
    mMaxWidth = maxWidth;
    mMaxHeight = 10000;
    mMaxWidth = maxWidth;
    mMaxHeight = maxHeight;
    mVertices[4] = (GLfloat) mMaxWidth;
    mVertices[8] = (GLfloat) mMaxHeight;
    mVertices[12] = (GLfloat) mMaxWidth;
    mVertices[13] = (GLfloat) mMaxHeight;
    mParentView = this;
}

ZView::ZView(Bounds maxWidth, Bounds maxHeight) {
    setOutlineColor(ZSettingsStore::getInstance().getInactiveColor());
    mMaxWidth = 10000;
    mMaxHeight = 10000;
    mMaxWidth = maxWidth;
    mMaxHeight = maxHeight;
    mVertices[4] = (GLfloat) mMaxWidth;
    mVertices[8] = (GLfloat) mMaxHeight;
    mVertices[12] = (GLfloat) mMaxWidth;
    mVertices[13] = (GLfloat) mMaxHeight;
    mParentView = this;
}

void ZView::onKeyPress(int key, int scancode, int action, int mods) {

    if ((key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) && action == GLFW_PRESS) {
        mShiftKeyPressed = true;
    } else if ((key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) && action == GLFW_RELEASE) {
        mShiftKeyPressed = false;
    }

    if ((key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) && action == GLFW_PRESS) {
        mAltKeyPressed = true;
    } else if ((key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) && action == GLFW_RELEASE) {
        mAltKeyPressed = false;
    }


    for (auto & mView : mViews) {
       // if ((isMouseInBounds(mView)) || action == GLFW_RELEASE) {
            mView->onKeyPress(key, scancode, action, mods);
       // }
    }
}

bool ZView::onMouseEvent(int button, int action, int mods, int x, int y) {
    vec2 scale = getScale();
    float sx = x / scale.x;
    float sy = y / scale.y;

    bool clickConsumed = false;
    if (getVisibility() && isClickable()) {

        for (ZView* view : mViews) {

            if (view  != nullptr) {

                if (isMouseInBounds(view)) {
                    clickConsumed |= view->onMouseEvent(button, action, mods, x, y);
                }

                if (action == GLFW_RELEASE && view->anyMouseDown()) {
                    view->onMouseEvent(button, action, mods, x, y);
                }

                if (action == GLFW_RELEASE) {
                    view->onMouseDrag(vec2(sx, sy), vec2(mMouseDownX, mMouseDownY),
                                vec2(sx - mMouseDownX, sy - mMouseDownY), mouseUp);
                }
            }
        }

        if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
            mMouseDown = false;
        }

        if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_RELEASE) {
            mMiddleMouseDown = false;
        }

        if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
            mRightMouseDown = false;
        }

        if (action == GLFW_RELEASE) {
            onMouseDrag(vec2(sx, sy), vec2(mMouseDownX, mMouseDownY),
                        vec2(sx - mMouseDownX, sy - mMouseDownY), mouseUp);
        }
        if (!clickConsumed) {
            if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
                mMouseDown = true;
            }

            if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS) {
                mMiddleMouseDown = true;
            }

            if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
                mRightMouseDown = true;
            }

            if (action == GLFW_PRESS) {
                onMouseDrag(vec2(sx, sy), vec2(mMouseDownX, mMouseDownY),
                            vec2(sx - mMouseDownX, sy - mMouseDownY), mouseDown);
                mMouseDownX = sx;
                mMouseDownY = sy;
            }


        }


    }

    return clickConsumed;
}

void ZView::setMiddleMouseDown(bool mouseDown) {
    mMiddleMouseDown = mouseDown;
}

void ZView::setRightMouseDown(bool mouseDown) {
    mRightMouseDown = mouseDown;
}

void ZView::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state) {
    if (state == mouseUp) {
        mMouseDragDelta = vec2(0);
    } else if (state == mouseDrag || state == mouseDown) {
        mMouseDragDelta = absolute - start;
    }
}

void ZView::onCursorPosChange(double x, double y) {
    vec2 scale = getScale();
    float sx = x / scale.x;
    float sy = y / scale.y;

    mLastX = mMouseX;
    mLastY = mMouseY;

    mMouseX = (int) sx;
    mMouseY = (int) sy;

    if (anyMouseDown()) {
        onMouseDrag(vec2(sx, sy), vec2(mMouseDownX, mMouseDownY),
                    vec2(sx - mMouseDownX, sy - mMouseDownY), mouseDrag);
    }

    for (auto & mView : mViews) {
        mView->onCursorPosChange(x, y);
    }
}

void ZView::setMouseDown(bool isDown) {
    mMouseDown = isDown;
}

bool ZView::isMouseInBounds(ZView *view) const {
    vec2 inner = view->getInnerTranslation();
    bool isInViewX = view->getLeft() - inner.x < view->getMouse().x  && view->getRight() - inner.x > view->getMouse().x ;
    bool isInViewY = view->getTop() - inner.y < view->getMouse().y && view->getBottom() - inner.x > view->getMouse().y;
    return isInViewX && isInViewY;
}

void ZView::onScrollChange(double x, double y) {
    if (getVisibility()) {
        for (auto view : mViews) {
            bool isInViewX = view->getLeft() < mMouseX && view->getRight() > mMouseX;
            bool isInViewY = view->getTop() < mMouseY && view->getBottom() > mMouseY;

            computeBounds();

            if (isInViewX && isInViewY) {
                view->onScrollChange(x, y);
            }
        }
    }
}

void ZView::onCreate() {
    init();
}

string ZView::getTag() {
    return mTag;
}

void ZView::setTag(string tag) {
    mTag = tag;
}

void ZView::draw() {
    ZShader* shader;
    if (mVisible) {
        if (mNeedsRender) {
            if (mBackgroundImage != nullptr) {
                mImageShader->use();
                shader = mImageShader;

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mBackgroundImage->getID());
            } else {
                mShader->use();
                shader = mShader;
            }

            // Update scale, useful for zooming a view out
            GLint vp_location = glGetUniformLocation(shader->mID, "uVPMatrix");
            mat4 projection = ortho(0.0f, (float) mWindowWidth, (float) mWindowHeight, 0.0f, -1.0f, 10.0f);

            vec2 absoluteScale = getScale();
            mat4 scaleMat = scale(projection, vec3(absoluteScale.x, absoluteScale.y, 0));
            glUniformMatrix4fv(vp_location, 1, GL_FALSE, glm::value_ptr(scaleMat));


            // Set the view background color
            glUniform4f(glGetUniformLocation(shader->mID, "uColor"),
                        mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, mBackgroundColor.a);

            glViewport(0, 0, mWindowWidth, mWindowHeight);

            glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
            glEnableVertexAttribArray(glGetAttribLocation(shader->mID, "vPosUi"));
            glVertexAttribPointer(glGetAttribLocation(shader->mID, "vPosUi"), 4, GL_FLOAT, GL_FALSE,
                                  sizeof(float) * 4, (void *) 0);

            glBindBuffer(GL_ARRAY_BUFFER, mTexBuffer);
            glEnableVertexAttribArray(glGetUniformLocation(shader->mID, "aTexCoords"));
            glVertexAttribPointer(glGetUniformLocation(shader->mID, "aTexCoords"), 2, GL_FLOAT, GL_FALSE,
                                  sizeof(float) * 2, (void *) 0);

            glLineWidth(2.0);
            if (mDrawWire == full) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEdgeIndicesBuffer);
                if (getVisibility()) {
                    glDrawElements(GL_LINES, EDGE_INDEX_COUNT, GL_UNSIGNED_INT, nullptr);
                }
            } else if (mDrawWire == outline) {
                glUniform4f(glGetUniformLocation(shader->mID, "uColor"),
                            mOutlineColor.r, mOutlineColor.g, mOutlineColor.b, mOutlineColor.a);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mOutlineIndicesBuffer);
                if (getVisibility()) {
                    glDrawElements(GL_LINES, OUTLINE_INDEX_COUNT, GL_UNSIGNED_INT, nullptr);
                }
                glUniform4f(glGetUniformLocation(shader->mID, "uColor"),
                            mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, mBackgroundColor.a);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);
                if (getVisibility()) {
                    glDrawElements(GL_TRIANGLES, FACE_INDEX_COUNT, GL_UNSIGNED_INT, nullptr);
                }
            } else {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);
                if (getVisibility()) {
                    glDrawElements(GL_TRIANGLES, FACE_INDEX_COUNT, GL_UNSIGNED_INT, nullptr);
                }
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        for (ZView* view : mViews) {
            view->draw();
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        mNeedsRender = false;
    }
}

void ZView::init() {
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float),  &mVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mFaceIndicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, FACE_INDEX_COUNT * sizeof(int), &mFaceIndices, GL_STATIC_DRAW);

    glGenBuffers(1, &mEdgeIndicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEdgeIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, EDGE_INDEX_COUNT * sizeof(int), &mEdgeIndices, GL_STATIC_DRAW);


    glGenBuffers(1, &mOutlineIndicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mOutlineIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, OUTLINE_INDEX_COUNT * sizeof(int), &mOutlineIndices, GL_STATIC_DRAW);


    glGenBuffers(1, &mTexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mTexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), mTexCoords, GL_STATIC_DRAW);

    computeBounds();
}

void ZView::setShader(ZShader *shader) {
    mShader = shader;

    mPositionLocation = glGetAttribLocation(shader->mID, "vPos");
    mColorLocation = glGetUniformLocation(shader->mID, "uColor");
    mTexCoordLocation = glGetUniformLocation(shader->mID, "aTexCoords");
    for (auto & mView : mViews) {
        mView->setShader(shader);
    }
}

void ZView::setTextShader(ZShader *shader) {
    mTextShader = shader;
    for (auto & mView : mViews) {
        mView->setTextShader(shader);
    }
}

ZShader* ZView::getTextShader() {
    return mTextShader;
}

void ZView::setImageShader(ZShader *shader) {
    mImageShader = shader;
    for (auto & mView : mViews) {
        mView->setImageShader(shader);
    }
}

ZShader* ZView::getImageShader() {
    return mImageShader;
}

void ZView::setWindowWidth(int width) {
    mWindowWidth = width;
    for (auto & mView : mViews) {
        mView->setWindowWidth(width);
    }
}

void ZView::setWindowHeight(int height) {
    mWindowHeight = height;
    for (auto & mView : mViews) {
        mView->setWindowHeight(height);
    }
}

void ZView::onWindowChange(int windowWidth, int windowHeight) {
    computeBounds();
    if (mParentView == this) {
        mMaxWidth = windowWidth;
        mMaxHeight = windowHeight;
    }

    for (auto & mView : mViews) {
        mView->onWindowChange(right, bottom);
    }

    invalidate();
}

// Called once before first draw
void ZView::onLayoutFinished(){
    for (ZView* view : mViews) {
        view->onLayoutFinished();
    }
}

void ZView::setMargin(int marginLeft, int marginTop, int marginRight, int marginBottom) {

    mMarginLeft = marginLeft;
    mMarginTop = marginTop;
    mMarginRight = marginRight;
    mMarginBottom = marginBottom;

    computeBounds();
    invalidate();
}

void ZView::setMargin(vec4 margin) {
    mMarginLeft = margin.x;
    mMarginTop = margin.y;
    mMarginRight =  margin.z;
    mMarginBottom =  margin.w;

    computeBounds();
    invalidate();
}

void ZView::setOffset(double x, double y) {
    mOffsetX = x;
    mOffsetY = y;
    computeBounds();
    invalidate();
}

void ZView::setOffset(vec2 pos) {
    setOffset(pos.x, pos.y);
}

void ZView::setXOffset(int x) {
    mOffsetX = x;
    invalidate();
}

void ZView::setYOffset(int y) {
    mOffsetY = y;
    invalidate();
}

void ZView::offsetBy(int x, int y) {
    mOffsetX += x;
    mOffsetY += y;
    computeBounds();
    invalidate();
}

double ZView::getOffsetX() {
    return mOffsetX;
}

double ZView::getOffsetY() {

    return mOffsetY;
}

int ZView::getMaxWidth() {
    return mMaxWidth;
}

int ZView::getMaxHeight() {
    return mMaxHeight;
}

int ZView::getHeight() {
    return getBottom() - getTop();
}

int ZView::getWidth() {
    return getRight() - getLeft();
}

int ZView::getMarginLeft() {
    return mMarginLeft;
}

int ZView::getMarginTop() {
    return mMarginTop;
}

int ZView::getMarginRight() {
    return mMarginRight;
}

int ZView::getMarginBottom() {
    return mMarginBottom;
}

void ZView::setMarginLeft(float m){
    mMarginLeft = m;
}

void ZView::setMarginTop(float m){
    mMarginTop = m;
}

void ZView::setMarginRight(float m){
    mMarginRight = m;
}

void ZView::setMarginBottom(float m){
    mMarginBottom = m;
}

void ZView::invalidate() {
    mNeedsRender = true;
    for (ZView* view : mViews) {
        view->invalidate();
    }
    //glfwPostEmptyEvent();
}

vec2 ZView::getCenter() {
    return vec2(((getRight() - getLeft()) / 2.0) + getLeft(),
            ((getBottom() - getTop()) / 2.0) + getTop());
}

void ZView::setBackgroundImage(ZTexture* background) {
    mBackgroundImage = background;
    setBackgroundColor(vec4(1));
}

int ZView::getWindowHeight() {
    return mWindowHeight;
}

int ZView::getWindowWidth() {
    return mWindowWidth;
}

void ZView::computeBounds() {
    calculateLeft();
    calculateTop();
    calculateBottom();
    calculateRight();

    mVertices[0] = getLeft();
    mVertices[1] = getTop();

    mVertices[4] = getRight();
    mVertices[5] = getTop();

    mVertices[8] = getLeft();
    mVertices[9] = getBottom();

    mVertices[12] = getRight();
    mVertices[13] = getBottom();

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 16 * sizeof(float), mVertices);

    glBindBuffer(GL_ARRAY_BUFFER, mTexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(float), mTexCoords);
}

void ZView::addSubView(ZView *view) {
    mViews.push_back(view);
    view->setParentView(this);
    view->setRootView(mRootView);

    if (mShader != nullptr) {
        view->setShader(mShader);
        view->setTextShader(mTextShader);
        view->setImageShader(mImageShader);
    }

    view->onCreate();
}

void ZView::clearSubViews() {
    for (auto & mView : mViews) {
        mView = NULL;
    }

    mViews.clear();
    mViews.shrink_to_fit();
}

void ZView::setParentView(ZView *parentView) {
    mParentView = parentView;
}

ZView* ZView::getParentView() {
    return mParentView;
}

bool ZView::mouseIsDown() {
    return mMouseDown;
}

bool ZView::middleMouseIsDown() {
    return mMiddleMouseDown;
}

bool ZView::rightMouseIsDown() {
    return mRightMouseDown;
}

bool ZView::shiftKeyPressed() {
    return mShiftKeyPressed;
}

bool ZView::altKeyPressed() {
    return mAltKeyPressed;
}

int ZView::getMouseDownX() {
    return mMouseDownX;
}

int ZView::getMouseDownY() {
    return mMouseDownY;
}

vec2 ZView::getMouse() {
    return vec2(mMouseX, mMouseY);
}

double ZView::getLastX() {
    return mLastX;
}

double ZView::getLastY() {
    return mLastY;
}

vector<ZView*> ZView::getSubViews() {
    return mViews;
}

void ZView::setBackgroundColor(float color[4]) {
    mBackgroundColor = vec4(color[0], color[1], color[2], color[3]);
    invalidate();
}

void ZView::setBackgroundColor(vec4 color) {
    mBackgroundColor = color;
    invalidate();
}

vec4 ZView::getBackgroundColor() {
    return mBackgroundColor;
}

void ZView::setGravity(Gravity gravity) {
    mGravity = gravity;
    invalidate();
}

void ZView::setMaxWidth(int width) {
    mMaxWidth = width;
    computeBounds();
}

void ZView::setMaxHeight(int height) {
    mMaxHeight = height;
    computeBounds();
}

bool ZView::anyMouseDown() {
    return mouseIsDown() || middleMouseIsDown() || rightMouseIsDown();
}

void ZView::onFileDrop(int count, const char** paths) {
    for (auto & mView : mViews) {
        mView->onFileDrop(count, paths);
    }
}

void ZView::onExit() {
    for (auto & mView : mViews) {
        mView->onExit();
    }
}

void ZView::setVisibility(bool visible) {
    mVisible = visible;
    invalidate();
}

bool ZView::getVisibility() {
    return mVisible;
}

bool ZView::needsRender() {
    return mNeedsRender;
}

int ZView::calculateLeft() {
    if (mParentView != this) {
        mParentView->calculateLeft();
    }

    int thisLeft = mOffsetX + mMarginLeft;
    vec2 translation = getInnerTranslation();
    float scale = getRelativeScale().x;
    float parentLeft = mParentView->getLeft() / scale + translation.x;
    float parentRight = mParentView->getRight() / scale + translation.x;

    if (mParentView == this) {
        mLeft = thisLeft;
        return mLeft;
    } else {
        if (mGravity == bottomRight || mGravity == topRight) {
            thisLeft = parentRight - mMarginRight - mMaxWidth - mOffsetX;

            if (thisLeft - mMarginLeft < parentLeft) {
                thisLeft = parentLeft + mMarginLeft;
            }

            mLeft = thisLeft;
            return mLeft;
        } else {
            if (thisLeft + parentLeft > parentRight) {
                mLeft = parentRight;
                return mLeft;
            }
            mLeft = parentLeft + thisLeft;
            return mLeft;
        }
    }
}

int ZView::calculateRight() {
    if (mParentView != this) {
        mParentView->calculateRight();
    }

    int thisRight = mMaxWidth + mMarginLeft + mOffsetX;

    vec2 translation = getInnerTranslation();
    float scale = getRelativeScale().x;
    float parentLeft = mParentView->getLeft() / scale + translation.x;
    float parentRight = mParentView->getRight() / scale + translation.x;

    if (mParentView == this) {
        mRight = thisRight;
        return mRight;
    } else {
        if (mGravity == topRight || mGravity == bottomRight) {
            thisRight = parentRight - mMarginRight - mOffsetX;

            if (thisRight < parentLeft + mMarginLeft) {
                thisRight = parentLeft + mMarginLeft;
            }
            mRight = thisRight;
            return mRight;
        }

        if (thisRight + parentLeft + mMarginRight < parentRight) {
            thisRight = parentLeft + thisRight;
        } else {
            thisRight = parentRight - mMarginRight;
        }

        if (thisRight < getLeft()) {
            thisRight = getLeft();
        }

        mRight = thisRight;
        return mRight;
    }

}

int ZView::calculateTop() {
    if (mParentView != this) {
        mParentView->calculateTop();
    }

    int thisTop = (int) (mOffsetY + mMarginTop);

    vec2 translation = getInnerTranslation();
    float parentTop = mParentView->getTop() / getRelativeScale().y  + translation.y;
    float parentBottom = mParentView->getBottom() / getRelativeScale().y + translation.y;

    if (mParentView == this) {
        mTop = thisTop;
        return mTop;
    } else {
        if (mGravity == bottomRight || mGravity == bottomLeft) {
            thisTop = (int) (parentBottom - mMarginBottom - mMaxHeight - mOffsetY);

            if (thisTop - mMarginTop < parentTop) {
                thisTop = (int) parentTop + mMarginTop;
            }

            mTop = thisTop;
            return mTop;
        } else {
            if (thisTop + parentTop > parentBottom) {
                mTop = (int) parentBottom;
                return mTop;
            }
            mTop = (int) parentTop + thisTop;
            return mTop;
        }
    }
}

int ZView::calculateBottom() {
    if (mParentView != this) {
        mParentView->calculateBottom();
    }

    vec2 translation = getInnerTranslation();
    float parentTop = mParentView->getTop() / getRelativeScale().y  + translation.y;
    float parentBottom = mParentView->getBottom() / getRelativeScale().y + translation.y;

    int thisBottom = (int) (mMaxHeight + mMarginTop + mOffsetY);

    if (mParentView == this) {
        mBottom = thisBottom;
        return mBottom;
    } else {
        if (mGravity == bottomLeft || mGravity == bottomRight) {
            thisBottom = parentBottom - mMarginBottom - mOffsetY;

            if (thisBottom < parentTop + mMarginTop) {
                thisBottom = parentTop + mMarginTop;
            }
            mBottom = thisBottom;
            return mBottom;
        }

        if (thisBottom + parentTop + mMarginTop < parentBottom) {
            thisBottom = parentTop + thisBottom;
        } else {
            thisBottom = parentBottom - mMarginBottom;
        }

        if (thisBottom < getTop()) {
            thisBottom = getTop();
        }

        mBottom = thisBottom;
        return thisBottom;
    }
}

int ZView::getLeft() {
    return mLeft;
}

int ZView::getTop() {
    return mTop;
}

int ZView::getRight() {
   return mRight;
}

int ZView::getBottom() {
    return mBottom;
}

vec4 ZView::getMargin() {
    return vec4(mMarginLeft, mMarginTop, mMarginRight, mMarginBottom);
}

void ZView::onGlobalMouseUp(int key) {
    for (auto view : mViews) {
        view->onGlobalMouseUp(key);
    }

    if (key == GLFW_MOUSE_BUTTON_1) {
        mMouseDown = false;
    }

    if (key == GLFW_MOUSE_BUTTON_2) {
        mRightMouseDown = false;
    }

    if (key == GLFW_MOUSE_BUTTON_3) {
        mMiddleMouseDown = false;
    }
}

void ZView::setDrawWire(WireType wire) {
    mDrawWire = wire;
}

float* ZView::getVertices() {
    return mVertices;
}

GLuint ZView::getVertexBuffer() {
    return mVertexBuffer;
}

int ZView::getIndexTag() {
    return mIndexTag;
}

void ZView::setIndexTag(int index) {
    mIndexTag = index;
}

void ZView::resetInitialPosition() {
    mInitialPosition = vec2(mOffsetX, mOffsetY);
}

vec2 ZView::getInitialPosition() {
    return mInitialPosition;
}

bool ZView::isClickable() {
    return mClickable;
}

void ZView::setClickable(bool clickable) {
    mClickable = clickable;
}

vec2 ZView::getMouseDragDelta() {
    return mMouseDragDelta;
}

void ZView::setInitialPosition(vec2 position) {
    mInitialPosition = position;
}

void ZView::setOutlineColor(vec4 color) {
    mOutlineColor = color;
}

vec2 ZView::getScale() {
    if (mParentView != nullptr && mParentView != this) {
        return mScale * mParentView->getScale();
    }
    return mScale;
}

void ZView::setScale(vec2 scale) {
    mScale = scale;
}

vec2 ZView::getInnerTranslation() {
    return mInnerTranslation;
}
void ZView::setInnerTranslation(vec2 scale) {
    mInnerTranslation = scale;
}

vec2 ZView::getRelativeScale() {
    return mScale;
}

vec2 ZView::getOffset() {
    return vec2(getOffsetX(), getOffsetY());
}

ZView* ZView::getRootView() {
    return mRootView;
}

void ZView::setRootView(ZView* rootView) {
    mRootView = rootView;
}

void ZView::incrementZCursor(){
    mZCursorPosition += 0.05;
}

float ZView::getZPosition(){
    return mZPosition;
}

void ZView::setZPosition(float z){
    mZPosition = z;
}

float ZView::getZCursorPosition(){
    return mZCursorPosition;
}

void ZView::setName(string name) {
    mName = name;
}

string ZView::getName() {
    return mName;
}
