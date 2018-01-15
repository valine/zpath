
#include "zview.h"


ZView::ZView(float maxWidth, float maxHeight, ZShader *shader) {

	mMaxWidth = maxWidth;
	mMaxHeight = maxHeight;

    mShader = shader;

    mVertices[0] = 0;
    mVertices[1] = 0;
    mVertices[2] = 0;

	mVertices[3] = mMaxWidth;
    mVertices[4] = 0;
    mVertices[5] = 0;

    mVertices[6] = 0;
    mVertices[7] = mMaxHeight;
	mVertices[8] = 0; 

    mVertices[9] = mMaxWidth;
	mVertices[10] = mMaxHeight;
	mVertices[11] = 0;

	mFaceIndices[0] = 0;
	mFaceIndices[1] = 1;
	mFaceIndices[2] = 2;

	mFaceIndices[3] = 1;
	mFaceIndices[4] = 2;
	mFaceIndices[5] = 3;

    mBackgroundColor[0] = 0.2;
    mBackgroundColor[1] = 0.2;
    mBackgroundColor[2] = 0.2;
    mBackgroundColor[3] = 0.2;

    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mFaceIndicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mFaceIndices) * sizeof(int), mFaceIndices, GL_STATIC_DRAW);

    mPositionLocation = glGetAttribLocation(shader->mID, "vPos");
    mColorLocation = glGetUniformLocation(shader->mID, "uColor");

    setParentView(this);
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

int ZView::getWidth() {
    return mMaxWidth;
}

int ZView::getHeight() {
    return mMaxHeight;
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
        if (thisTop + mParentView->getTop() > mParentView->getBottom()) {
            return mParentView->getBottom();
        }
        return mParentView->getTop() + thisTop;
    }
}

int ZView::getRight() {
    int thisRight = mMaxWidth + mMarginLeft + mOffsetX;

    if (mGravity == bottomRight || mGravity == topRight) {
        thisRight += mParentView->getWidth() - mMaxWidth - mMarginRight;
    }

    if (mParentView == this) {
        return thisRight;
    } else {
          if (thisRight + mParentView->getLeft() < mParentView->getRight()) {
             return mParentView->getLeft() + thisRight;
        } else {
            return mParentView->getRight() - mMarginRight;
        }
    }

}

int ZView::getBottom() {
    int thisBottom = mMaxHeight + mMarginTop + mOffsetY;
    if (mParentView == this) {
        return thisBottom;
    } else {
        if (thisBottom + mParentView->getTop() < mParentView->getBottom()) {
             return mParentView->getTop() + thisBottom;
        } else {
            return mParentView->getBottom() - mMarginBottom;
        }
    }
}


void ZView::computeBounds(int windowWidth, int windowHeight) {


        // if (mGravity == bottomRight || mGravity == topRight) {
        //     int width = right - left + (mMarginRight * 2);
        //     left += mParentView->getWidth() - width;
        //     right += mParentView->getWidth() - width;
        // }

        // if (mGravity == bottomLeft || mGravity == bottomRight) {
        //     int height = bottom - top + (mMarginBottom * 2);
        //     top += windowHeight - height;
        //     bottom += windowHeight - height;
        // }

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
}

void ZView::setParentView(ZView *parentView) {
    mParentView = parentView;
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

void ZView::draw() {

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);

    glEnableVertexAttribArray(mPositionLocation);
    glVertexAttribPointer(mPositionLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 3, (void*) 0);

     glProgramUniform4f(mShader->mID, mColorLocation,
        mBackgroundColor[0], mBackgroundColor[1], 
        mBackgroundColor[2], mBackgroundColor[3]);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); 

    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->draw();
    }
}
