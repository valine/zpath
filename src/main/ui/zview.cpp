
#include "zview.h"


ZView::ZView(float maxWidth, float maxHeight) {

    init(maxWidth, maxHeight);
}

ZView::ZView(Bounds maxWidth, float maxHeight) {
    init(100000, maxHeight);
}

ZView::ZView(float maxWidth, Bounds maxHeight) {
    init(maxWidth, 100000);
}

ZView::ZView(Bounds maxWidth, Bounds maxHeight) {
    init(100000, 100000);
}


void ZView::draw() {
    mShader->use();
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);

    glEnableVertexAttribArray(mPositionLocation);
    glVertexAttribPointer(mPositionLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 3, (void*) 0);

     glProgramUniform4f(mShader->mID, mColorLocation,
        mBackgroundColor[0], mBackgroundColor[1], 
        mBackgroundColor[2], mBackgroundColor[3]);

   // if (mParentView != this) {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
   //}

    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->draw();
    }
}

void ZView::init(int maxWidth, int maxHeight) {
    mMaxWidth = maxWidth;
    mMaxHeight = maxHeight;

    mVertices[3] = mMaxWidth;
    mVertices[7] = mMaxHeight;

    mVertices[9] = mMaxWidth;
    mVertices[10] = mMaxHeight;

    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mFaceIndicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mFaceIndices) * sizeof(int), mFaceIndices, GL_STATIC_DRAW);

    setParentView(this);
}


void ZView::setShader(ZShader *shader) {

    mShader = shader;

    mPositionLocation = glGetAttribLocation(shader->mID, "vPos");
    mColorLocation = glGetUniformLocation(shader->mID, "uColor");

     for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->setShader(shader);
    }
}

void ZView::onWindowChange(int windowWidth, int windowHeight) {
    if (mParentView == this) {
        mMaxWidth = windowWidth;
        mMaxHeight = windowHeight;
    }

    computeBounds(windowWidth, windowHeight);
    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->onWindowChange(right, bottom);
    }
}

void ZView::setMargin(int marginLeft, int marginTop, int marginRight, int marginBottom) {

    mMarginLeft = marginLeft;
    mMarginTop = marginTop;
    mMarginRight = marginRight;
    mMarginBottom = marginBottom;

    computeBounds(mParentWidth, mParentWidth);
}

void ZView::setOffset(int x, int y) {
    mOffsetX = x;
    mOffsetY = y;
}

int ZView::getOffsetX() {
    if (mParentView == this) {
        return mOffsetX;
    }
    return mOffsetX  + mParentView->getOffsetX();
}

int ZView::getOffsetY() {
    if (mParentView == this) {
        return mOffsetY; 
    } 
    return mOffsetY + mParentView->getOffsetY();
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

int ZView::getLeft() {
    int thisLeft = mOffsetX + mMarginLeft;

    if (mParentView == this) {
        return thisLeft;
    } else {
        if (mGravity == bottomRight || mGravity == topRight) {
            thisLeft = mParentView->getRight() - mMarginRight - mMaxWidth - mOffsetX;

            if (thisLeft - mMarginLeft < mParentView->getLeft()) {
                thisLeft = mParentView->getLeft() + mMarginLeft;
            }

            return thisLeft;
        } else {
            if (thisLeft + mParentView->getLeft() > mParentView->getRight()) {
                return mParentView->getRight();
            }
            return mParentView->getLeft() + thisLeft; 
        }
    }
}

int ZView::getTop() {

    int thisTop = mOffsetY + mMarginTop;

    if (mParentView == this) {
        return thisTop;
    } else {
        if (mGravity == bottomRight || mGravity == bottomLeft) {
            thisTop = mParentView->getBottom() - mMarginBottom - mMaxHeight - mOffsetY;

            if (thisTop - mMarginTop < mParentView->getTop()) {
                thisTop = mParentView->getTop() + mMarginTop;
            }

            return thisTop;
        } else {
            if (thisTop + mParentView->getTop() > mParentView->getBottom()) {
                return mParentView->getBottom();
            }
            return mParentView->getTop() + thisTop; 
        }
    }
}

int ZView::getRight() {
    int thisRight = mMaxWidth + mMarginLeft + mOffsetX;

    if (mParentView == this) {
        return thisRight;
    } else {
        if (mGravity == topRight || mGravity == bottomRight) {
            thisRight = mParentView->getRight() - mMarginRight - mOffsetX;

            if (thisRight < mParentView->getLeft() + mMarginLeft) {
                thisRight = mParentView->getLeft() + mMarginLeft;
            }
            return thisRight;
        }

        if (thisRight + mParentView->getLeft() + mMarginRight < mParentView->getRight()) {
             thisRight = mParentView->getLeft() + thisRight;
        } else {
            thisRight = mParentView->getRight() - mMarginRight;
        }

        if (thisRight < getLeft()) {
            thisRight = getLeft();
        } 

        return thisRight;
    }

}

int ZView::getBottom() {
    int thisBottom = mMaxHeight + mMarginTop + mOffsetY;

    if (mParentView == this) {
        return thisBottom;
    } else {
        if (mGravity == bottomLeft || mGravity == bottomRight) {
            thisBottom = mParentView->getBottom() - mMarginBottom - mOffsetY;

            if (thisBottom < mParentView->getTop() + mMarginTop) {
                thisBottom = mParentView->getTop() + mMarginTop;
            }
            return thisBottom;
        }

        if (thisBottom + mParentView->getTop() + mMarginTop < mParentView->getBottom()) {
             thisBottom = mParentView->getTop() + thisBottom;
        } else {
            thisBottom = mParentView->getBottom() - mMarginBottom;
        }

        if (thisBottom < getTop()) {
            thisBottom = getTop();
        } 

        return thisBottom;
    }
}

void ZView::computeBounds(int windowWidth, int windowHeight) {
        mVertices[0] = getLeft();
        mVertices[1] = getTop();

        mVertices[3] = getRight();
        mVertices[4] = getTop();

        mVertices[6] = getLeft();
        mVertices[7] = getBottom();

        mVertices[9] = getRight();
        mVertices[10] = getBottom();

        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices, GL_STATIC_DRAW);
    
}

void ZView::addSubView(ZView *view) {
    mViews.push_back(view);
    view->setParentView(this);

    if (mShader != nullptr) {
        view->setShader(mShader);
    }
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


int ZView::getMouseDownX() {
    return mMouseDownX;
}

int ZView::getMouseDownY() {
    return mMouseDownY;
}

vector<ZView*> ZView::getSubViews() {
    return mViews;
}

void ZView::setBackgroundColor(float color[4]) {
    mBackgroundColor[0] = color[0];
    mBackgroundColor[1] = color[1];
    mBackgroundColor[2] = color[2];
    mBackgroundColor[3] = color[3];
}

void ZView::setGravity(Gravity gravity) {
    mGravity = gravity;
}

void ZView::setMaxWidth(int width) {
    mMaxWidth = width;
}

void ZView::setMaxHeight(int height) {
    mMaxHeight = height;
}

void ZView::onKeyPress(int key, int scancode, int action, int mods) {
    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->onKeyPress(key, scancode, action, mods);
    }
}

void ZView::onMouseEvent(int button, int action, int mods, int x, int y) {
    if (action == GLFW_PRESS) {
        mMouseDown  = true;
        mMouseDownX = x;
        mMouseDownY = y;
    } else {
        mMouseDown = false;
    }

    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        ZView* view = (*it);

        if (action == GLFW_PRESS && view->getLeft() < mMouseX && view->getRight() > mMouseX) {
            view->onMouseEvent(button, action, mods, x, y);
        } 

        if (action == GLFW_RELEASE && view->mouseIsDown()) {
            view->onMouseEvent(button, action, mods, x, y);
        }
       
    }
}

void ZView::onCursorPosChange(double x, double y) {

    mMouseX = x;
    mMouseY = y;
    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->onCursorPosChange(x, y);
    }
}

