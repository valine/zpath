#include "ui/zview.h"

ZView::ZView(float maxWidth, float maxHeight) {
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
    mMaxWidth = maxWidth;
    mMaxHeight = maxHeight;
    mMaxWidth = maxWidth;
    mMaxHeight = maxHeight;
    mVertices[4] = (GLfloat) mMaxWidth;
    mVertices[8] = (GLfloat) mMaxHeight;
    mVertices[12] = (GLfloat) mMaxWidth;
    mVertices[13] = (GLfloat) mMaxHeight;
    parent->addSubView(this);
}

ZView::ZView(Bounds maxWidth, float maxHeight) {
    mMaxWidth = 100000;
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
    mMaxWidth = maxWidth;
    mMaxHeight = 100000;
    mMaxWidth = maxWidth;
    mMaxHeight = maxHeight;
    mVertices[4] = (GLfloat) mMaxWidth;
    mVertices[8] = (GLfloat) mMaxHeight;
    mVertices[12] = (GLfloat) mMaxWidth;
    mVertices[13] = (GLfloat) mMaxHeight;
    mParentView = this;
}

ZView::ZView(Bounds maxWidth, Bounds maxHeight) {
    mMaxWidth = 100000;
    mMaxHeight = 100000;
    mMaxWidth = maxWidth;
    mMaxHeight = maxHeight;
    mVertices[4] = (GLfloat) mMaxWidth;
    mVertices[8] = (GLfloat) mMaxHeight;
    mVertices[12] = (GLfloat) mMaxWidth;
    mVertices[13] = (GLfloat) mMaxHeight;
    mParentView = this;
}

void ZView::onCreate() {
    init((int) mMaxWidth, (int) mMaxHeight);
    computeBounds();
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

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);

//            if (mParentView != this) {
                if (getVisibility()) {
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
                }
           // }

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

void ZView::init(int maxWidth, int maxHeight) {
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float),  &mVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mFaceIndicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int), &mFaceIndices, GL_STATIC_DRAW);

    glGenBuffers(1, &mTexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mTexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), mTexCoords, GL_STATIC_DRAW);

}

void ZView::setShader(ZShader *shader) {
    mShader = shader;

    mPositionLocation = glGetAttribLocation(shader->mID, "vPos");
    mColorLocation = glGetUniformLocation(shader->mID, "uColor");
    mTexCoordLocation = glGetUniformLocation(shader->mID, "aTexCoords");
    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->setShader(shader);
    }
}

void ZView::setTextShader(ZShader *shader) {
    mTextShader = shader;
    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->setTextShader(shader);
    }
}

ZShader* ZView::getTextShader() {
    return mTextShader;
}


void ZView::setImageShader(ZShader *shader) {
    mImageShader = shader;
    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->setImageShader(shader);
    }
}

ZShader* ZView::getImageShader() {
    return mImageShader;
}

void ZView::setWindowWidth(int width) {
    mWindowWidth = width;
    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->setWindowWidth(width);
    }
}

void ZView::setWindowHeight(int height) {
    mWindowHeight = height;
    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->setWindowHeight(height);
    }
}

void ZView::onWindowChange(int windowWidth, int windowHeight) {
    computeBounds();
    if (mParentView == this) {
        mMaxWidth = windowWidth;
        mMaxHeight = windowHeight;
    }

    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->onWindowChange(right, bottom);
    }

    invalidate();
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

void ZView::invalidate() {
    mNeedsRender = true;
    for (ZView* view : mViews) {
        view->invalidate();
    }
    //glfwPostEmptyEvent();
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

    if (mShader != nullptr) {
        view->setShader(mShader);
        view->setTextShader(mTextShader);
        view->setImageShader(mImageShader);
    }

    view->onCreate();
}

void ZView::clearSubViews() {
    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it) = NULL;
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

        for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
            bool isInViewX = (*it)->getLeft() < mMouseX && (*it)->getRight() > mMouseX;
            bool isInViewY = (*it)->getTop() < mMouseY && (*it)->getBottom() > mMouseY;

            if ((isInViewX && isInViewY) || action == GLFW_RELEASE) {
                (*it)->onKeyPress(key, scancode, action, mods);
            }
        }
    
}

void ZView::onMouseEvent(int button, int action, int mods, int x, int y) {
    if (getVisibility()) {
        if (action == GLFW_PRESS) {

            mMouseDown  = true;
            mMouseDownX = x;
            mMouseDownY = y;
        } else {
            mMouseDown = false;
        }

        if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_RELEASE) {
            mMiddleMouseDown = false;
        } else if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS) {
            mMiddleMouseDown = true;
        }

        for (auto view : mViews) {
            bool isInViewX = view->getLeft() < mMouseX && view->getRight() > mMouseX;
            bool isInViewY = view->getTop() < mMouseY && view->getBottom() > mMouseY;

            if (isInViewY && isInViewX) {
                view->onMouseEvent(button, action, mods, x, y);
            }

            if (action == GLFW_RELEASE && view->mouseIsDown()) {
                view->onMouseEvent(button, action, mods, x, y);
            }
        }
    }
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
}

bool ZView::getVisibility() {
    return mVisible;
}

void ZView::onScrollChange(double x, double y) {
    if (getVisibility()) {
        for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
            ZView* view = (*it);

            bool isInViewX = view->getLeft() < mMouseX && view->getRight() > mMouseX;
            bool isInViewY = view->getTop() < mMouseY && view->getBottom() > mMouseY;

            computeBounds();

            if (isInViewX && isInViewY) {
                view->onScrollChange(x, y);
            }
        }
    }
}

void ZView::onCursorPosChange(double x, double y) {
    mLastX = mMouseX;
    mLastY = mMouseY;

    mMouseX = x;
    mMouseY = y;
    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->onCursorPosChange(x, y);
    }
}

bool ZView::needsRender() {
    return mNeedsRender;
}

int ZView::calculateLeft() {
    if (mParentView != this) {
        mParentView->calculateLeft();
    }

    int thisLeft = mOffsetX + mMarginLeft;

    if (mParentView == this) {
        mLeft = thisLeft;
        return mLeft;
    } else {
        if (mGravity == bottomRight || mGravity == topRight) {
            thisLeft = mParentView->getRight() - mMarginRight - mMaxWidth - mOffsetX;

            if (thisLeft - mMarginLeft < mParentView->getLeft()) {
                thisLeft = mParentView->getLeft() + mMarginLeft;
            }

            mLeft = thisLeft;
            return mLeft;
        } else {
            if (thisLeft + mParentView->getLeft() > mParentView->getRight()) {
                mLeft = mParentView->getRight();
                return mLeft;
            }
            mLeft = mParentView->getLeft() + thisLeft;
            return mLeft;
        }
    }
}


int ZView::calculateRight() {
    if (mParentView != this) {
        mParentView->calculateRight();
    }

    int thisRight = mMaxWidth + mMarginLeft + mOffsetX;

    if (mParentView == this) {
        mRight = thisRight;
        return mRight;
    } else {
        if (mGravity == topRight || mGravity == bottomRight) {
            thisRight = mParentView->getRight() - mMarginRight - mOffsetX;

            if (thisRight < mParentView->getLeft() + mMarginLeft) {
                thisRight = mParentView->getLeft() + mMarginLeft;
            }
            mRight = thisRight;
            return mRight;
        }

        if (thisRight + mParentView->getLeft() + mMarginRight < mParentView->getRight()) {
            thisRight = mParentView->getLeft() + thisRight;
        } else {
            thisRight = mParentView->getRight() - mMarginRight;
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

    int thisTop = mOffsetY + mMarginTop;

    if (mParentView == this) {
        mTop = thisTop;
        return mTop;
    } else {
        if (mGravity == bottomRight || mGravity == bottomLeft) {
            thisTop = mParentView->getBottom() - mMarginBottom - mMaxHeight - mOffsetY;

            if (thisTop - mMarginTop < mParentView->getTop()) {
                thisTop = mParentView->getTop() + mMarginTop;
            }

            mTop = thisTop;
            return mTop;
        } else {
            if (thisTop + mParentView->getTop() > mParentView->getBottom()) {
                mTop = mParentView->getBottom();
                return mTop;
            }
            mTop = mParentView->getTop() + thisTop;
            return mTop;
        }
    }
}


int ZView::calculateBottom() {
    if (mParentView != this) {
        mParentView->calculateBottom();
    }

    int thisBottom = mMaxHeight + mMarginTop + mOffsetY;

    if (mParentView == this) {
        mBottom = thisBottom;
        return mBottom;
    } else {
        if (mGravity == bottomLeft || mGravity == bottomRight) {
            thisBottom = mParentView->getBottom() - mMarginBottom - mOffsetY;

            if (thisBottom < mParentView->getTop() + mMarginTop) {
                thisBottom = mParentView->getTop() + mMarginTop;
            }
            mBottom = thisBottom;
            return mBottom;
        }

        if (thisBottom + mParentView->getTop() + mMarginTop < mParentView->getBottom()) {
            thisBottom = mParentView->getTop() + thisBottom;
        } else {
            thisBottom = mParentView->getBottom() - mMarginBottom;
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


