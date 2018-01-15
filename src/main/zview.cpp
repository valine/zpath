
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
}

void ZView::onWindowChange(int windowWidth, int windowHeight) {

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

    for (vector<ZView*>::iterator it = mViews.begin() ; it != mViews.end(); ++it) {
        (*it)->setOffset(x, y);
    }
}

void ZView::computeBounds(int windowWidth, int windowHeight) {

    if (windowWidth != mParentWidth || windowHeight != mParentHeight) {
        mParentWidth = windowWidth;
        mParentHeight = windowHeight;

        left = mOffsetX + mMarginLeft; // TODO: Surrounding view need to be accounted for
        top = mOffsetY + mMarginTop; // TODO: Surrounding view need to be accounted for

        if (windowWidth < mMaxWidth + mOffsetX) {
            right = windowWidth - mMarginRight;
        } else {
            right = mMaxWidth + mOffsetX - mMarginRight;
        }

        if (windowHeight < mMaxHeight + mOffsetY) {
            bottom = windowHeight - mMarginBottom;
        } else {
            bottom = mMaxHeight + mOffsetY - mMarginBottom;
        }

        mVertices[0] = left;
        mVertices[1] = top;

        mVertices[3] = right;
        mVertices[4] = top;

        mVertices[6] = left;
        mVertices[7] = bottom;

        mVertices[9] = right;
        mVertices[10] = bottom;

        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices, GL_STATIC_DRAW);
    }
}

void ZView::addSubView(ZView *view) {
    mViews.push_back(view);
    view->setOffset(mOffsetX + mMarginLeft, mOffsetY + mMarginTop);
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
