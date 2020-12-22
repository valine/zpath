#include <utility>

//
// Created by lukas on 12/5/20.
//

#include "ui/zlinechart.h"

ZLineChart::ZLineChart(float width, float height, ZView *parent) : ZView(width, height, parent) {
    mShader = new ZShader(ui_vs, ui_fs);

    glGenFramebuffers(1, &mFBO);
    glGenTextures(1, &mTexBuffer);
    glGenRenderbuffers(1, &mRBO);

    glGenTextures(1, &mFinalTexBuffer);
    glGenFramebuffers(1, &mFinalFBO);
    glGenRenderbuffers(1, &mFinalRBO);

    mBackground = new ZTexture(mFinalTexBuffer);
    setBackgroundImage(mBackground);

    mTmpTransform = ortho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 10.0f);
    mTransform = mDefaultMat;

    updateFBOSize();
    addGrid();
    addBackgroundGrid();
}

void ZLineChart::onWindowChange(int width, int height) {
    bool needsUpdate = false;
    if (width != getWidth() || height != getHeight()) {
        needsUpdate = true;
    }
    ZView::onWindowChange(width, height);

    if (needsUpdate) {
        updateFBOSize();
       // draw();
    }
}

void ZLineChart::updateFBOSize() {
    int width = getWidth();
    int height = getHeight();
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mTexBuffer);

    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA, width, height, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH_COMPONENT, width, height);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mTexBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mTexBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, mFinalFBO);
    glBindTexture(GL_TEXTURE_2D, mFinalTexBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindRenderbuffer(GL_RENDERBUFFER, mFinalRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFinalTexBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mFinalTexBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ZLineChart::updateLineBuffers() {

    if (mListener == nullptr) {
        return;
    }

    if (mInputCount == 1) {
        updateChart1D();
    } else {
        updateChart2D();
    }

    // Reset the tmp transform. The tmp transform is used while evaluation
    // happens on a background thread. This ensures the ui will not lag
    // when evaluating large graphs.
    mTmpTransform = ortho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 10.0f);
}

void ZLineChart::updateChart2D() {
    vector<float> verts;
    vector<int> edges;

    for (int lineIndex = 0; lineIndex < mLineCount; lineIndex++) {
        vector<float> output;
        for (uint i = 0; i < mResolution; i++) {
            output = mListener({(int) i}, lineIndex);
            verts.push_back(output.at(0));
            verts.push_back(output.at(1));
            verts.push_back(0);
            verts.push_back(0);

            if (i != mResolution - 1) {
                edges.push_back(i);
                edges.push_back(i + 1);
            }
        }

        // Initialize buffers
        if (mPoints.size() <= lineIndex) {
            mPointCount.push_back(edges.size());
            unsigned int lineBuffer;
            glGenBuffers(1, &lineBuffer);
            mPoints.push_back(lineBuffer);

            unsigned int edgeBuffer;
            glGenBuffers(1, &edgeBuffer);
            mEdges.push_back(edgeBuffer);
        } else {
            mPointCount.at(lineIndex) = edges.size();
        }

        glBindBuffer(GL_ARRAY_BUFFER, mPoints.at(lineIndex));
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEdges.at(lineIndex));
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(int), &edges[0], GL_DYNAMIC_DRAW);


        //mat4 tmpMat = mMatListener();
        // Reset the tmp transform. The tmp transform is used while evaluation
        // happens on a background thread. This ensures the ui will not lag
        // when evaluating large graphs.


       // mTmpTransform = (inverse(tmpMat) * mTmpTransform);
       // mTmpTransformIdentity = mat4(1);
    }

}


void ZLineChart::updateChart1D() {
    vector<float> verts;
    vector<int> edges;

    for (int lineIndex = 0; lineIndex < mLineCount; lineIndex++) {
        for (uint i = 0; i < mResolution; i++) {
            vector<float> y = mListener({(int) i}, lineIndex);
            verts.push_back(((float) i / (float) (mResolution - 1)));
            verts.push_back(y.at(0));
            verts.push_back(0);
            verts.push_back(0);

            if (i != mResolution - 1) {
                edges.push_back(i);
                edges.push_back(i + 1);
            }
        }

        // Initialize buffers
        if (mPoints.size() <= lineIndex) {
            mPointCount.push_back(edges.size());
            unsigned int lineBuffer;
            glGenBuffers(1, &lineBuffer);
            mPoints.push_back(lineBuffer);

            unsigned int edgeBuffer;
            glGenBuffers(1, &edgeBuffer);
            mEdges.push_back(edgeBuffer);
        } else {
            mPointCount.at(lineIndex) = edges.size();
        }

        glBindBuffer(GL_ARRAY_BUFFER, mPoints.at(lineIndex));
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEdges.at(lineIndex));
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(int), &edges[0], GL_DYNAMIC_DRAW);
    }
}

void ZLineChart::addBackgroundGrid() {
    vector<float> verts;
    vector<int> edges;

    int index = 0;
    for (float x = -10; x < 10; x += 0.1) {
        verts.push_back(-1e7);
        verts.push_back(x);
        verts.push_back(0);
        verts.push_back(0);

        verts.push_back(1e7);
        verts.push_back(x);
        verts.push_back(0);
        verts.push_back(0);

        edges.push_back(index);
        edges.push_back(index+1);
        index+=2;
    }

    for (float y = -10; y < 10; y += 0.1) {
        verts.push_back(y);
        verts.push_back(-1e7);
        verts.push_back(0);
        verts.push_back(0);

        verts.push_back(y);
        verts.push_back(1e7);
        verts.push_back(0);
        verts.push_back(0);

        edges.push_back(index);
        edges.push_back(index+1);
        index+=2;
    }

    mBGridVCount = verts.size() / 4;

    glGenBuffers(1, &mBGridVertBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mBGridVertBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);

    glGenBuffers(1, &mBGridEdgeBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBGridEdgeBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(int), &edges[0], GL_DYNAMIC_DRAW);
}

void ZLineChart::addGrid() {
    vector<float> verts = {-1e7, 0, 0, 0,
                           1e7, 0, 0, 0,
                           0, -1e7, 0, 0,
                           0, 1e7, 0, 0};
    vector<int> edges = {0, 1, 2, 3};
    glGenBuffers(1, &mGridVertBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mGridVertBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);

    glGenBuffers(1, &mGridEdgeBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGridEdgeBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(int), &edges[0], GL_DYNAMIC_DRAW);
}

void ZLineChart::draw() {

    if (mLineUpdatedNeeded) {
        mLineUpdatedNeeded = false;
        updateLineBuffers();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    mShader->use();

   // mat4 projection = mTmpTransform;
    mShader->setMat4("uVPMatrix", mTransform);

    glViewport(0, 0, getWidth(), getHeight());

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw background grid
    mShader->setVec4("uColor", grey);
    glLineWidth(1.0);
    glBindBuffer(GL_ARRAY_BUFFER, mBGridVertBuffer);
    glEnableVertexAttribArray(glGetAttribLocation(mShader->mID, "vPosUi"));
    glVertexAttribPointer(glGetAttribLocation(mShader->mID, "vPosUi"), 4, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 4, (void*) 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBGridEdgeBuffer);
    glDrawElements(GL_LINES, mBGridVCount, GL_UNSIGNED_INT, nullptr);


    // draw grid
    mShader->setVec4("uColor", red);
    glBindBuffer(GL_ARRAY_BUFFER, mGridVertBuffer);
    glEnableVertexAttribArray(glGetAttribLocation(mShader->mID, "vPosUi"));
    glVertexAttribPointer(glGetAttribLocation(mShader->mID, "vPosUi"), 4, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 4, (void*) 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGridEdgeBuffer);
    glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, nullptr);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFinalFBO);
    glBlitFramebuffer(0, 0, getWidth(), getHeight(), 0, 0, getWidth(), getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // mat4 projection = mTmpTransform;
    mShader->setMat4("uVPMatrix", mTmpTransform);

    // Draw graph lines
    glLineWidth(mLineWidth);
    glDepthMask(false);
    for (uint i = 0; i < mPoints.size(); i++) {
        mShader->setVec4("uColor", vec4(1.0, 0.0, 0.0, 1.0) *
                                   vec4(vec3((float) i / mPoints.size()), 1.0));
        glBindBuffer(GL_ARRAY_BUFFER, mPoints.at(i));
        glEnableVertexAttribArray(glGetAttribLocation(mShader->mID, "vPosUi"));
        glVertexAttribPointer(glGetAttribLocation(mShader->mID, "vPosUi"), 4, GL_FLOAT, GL_FALSE,
                              sizeof(float) * 4, (void*) 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEdges.at(i));
        glDrawElements(GL_LINES, mPointCount.at(i), GL_UNSIGNED_INT, nullptr);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ZView::draw();
}

mat<4, 4, float> ZLineChart::getMatrix() const {
    return mTmpTransform;
}

void ZLineChart::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state) {
    ZView::onMouseDrag(absolute,start, delta, state);

    vec2 corner = vec2(getLeft(), getTop());
    vec2 positionInView = absolute - corner;

    bool needsRefresh = false;
    /**
     * Origin is determined by the mouse down quadrant. The graph should scale to the opposite corner
     */
    if (state == mouseDown && rightMouseIsDown()) {
        if (positionInView.x > getWidth() / 2.0) {
            mScaleOrigin.x = -1;
        } else {
            mScaleOrigin.x = 1;
            positionInView.x = getWidth() - positionInView.x;
        }

        if (positionInView.y > getHeight() / 2.0) {
            mScaleOrigin.y = -1;
        } else {
            mScaleOrigin.y = 1;
            positionInView.y = getHeight() - positionInView.y;
        }
    }

    if (state == mouseDown) {
        mLastMouse = positionInView;
    }

    if (middleMouseIsDown() && shiftKeyPressed()) {
        if (state == mouseDrag) {
            vec2 d = (mLastMouse - positionInView) * vec2(2.0);
            mTmpTransformIdentity = translate(mat4(1), vec3(d.x / -getWidth(), d.y / -getHeight(), 0)) * mTmpTransformIdentity;
            mTmpTransform = translate(mat4(1), vec3(d.x / -getWidth(), d.y / -getHeight(), 0)) * mTmpTransform;
            mTransform = translate(mat4(1), vec3(d.x / -getWidth(), d.y / -getHeight(), 0)) * mTransform;
            needsRefresh = true;
        }

        mLastMouse = positionInView;
    } else if (rightMouseIsDown() && shiftKeyPressed()) {
        vec2 minPos = vec2(1,1);

        vec4 p1 = vec4(0,0,0,1);
        vec4 p2 = vec4(0.5,0.5,0,1);
        vec4 sampleScale = ((mTransform * p2) - (mTransform * p1)) / vec4(1,-1,0,0);
        vec4 sampleTranslation = (mTransform * p1);

        if (mScaleOrigin.x > 0) {
            positionInView.x = getWidth() - positionInView.x;
        }

        if (mScaleOrigin.y > 0) {
            positionInView.y = getHeight() - positionInView.y;
        }

        vec2 percent = positionInView / mLastMouse;

        // Handle zoom
        if (state == mouseDrag) {

            if (positionInView.x < minPos.x) {
                percent.x = 0.01 / sampleScale.x;
            } else {
                mLastMouse.x = positionInView.x;
            }

            if (positionInView.y < minPos.y) {
                percent.y = 0.01 / sampleScale.y;
            } else {
                mLastMouse.y = positionInView.y;
            }
            mTmpTransformIdentity = (translate(mat4(1), mScaleOrigin) *
                             scale(mat4(1), vec3(percent.x, percent.y, 1)) *
                             translate(mat4(1), -mScaleOrigin)) * mTmpTransformIdentity;
            mTmpTransform = (translate(mat4(1), mScaleOrigin) *
                             scale(mat4(1), vec3(percent.x, percent.y, 1)) *
                             translate(mat4(1), -mScaleOrigin)) * mTmpTransform;

            mTransform = (translate(mat4(1), mScaleOrigin) *
                             scale(mat4(1), vec3(percent.x, percent.y, 1)) *
                             translate(mat4(1), -mScaleOrigin)) * mTransform;
            needsRefresh = true;

        }
    }

    if (state == mouseUp) {
        needsRefresh = true;
    }

    if (needsRefresh && mInvalidateListener != nullptr) {
        mInvalidateListener();
    }

    invalidate();
}

void ZLineChart::onScrollChange(double x, double y) {
    ZView::onScrollChange(x,y);

    float speed = 0.1; // higher is faster
    if (shiftKeyPressed()) {
        float factor = 1.0;
        if (y > 0) {
            factor += speed;
        } else {
            factor -= speed;
        }

        vec3 origin = vec3(0);
        vec2 percent = vec2(factor);
        mTmpTransformIdentity = (translate(mat4(1), origin) *
                         scale(mat4(1), vec3(percent.x, percent.y, 1)) *
                         translate(mat4(1), -origin)) * mTmpTransformIdentity;
        mTmpTransform = (translate(mat4(1), origin) *
                         scale(mat4(1), vec3(percent.x, percent.y, 1)) *
                         translate(mat4(1), -origin)) * mTmpTransform;
        mTransform = (translate(mat4(1), origin) *
                         scale(mat4(1), vec3(percent.x, percent.y, 1)) *
                         translate(mat4(1), -origin)) * mTransform;
        if (mInvalidateListener != nullptr) {
            mInvalidateListener();
        }
    }


    invalidate();
}
