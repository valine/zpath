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

    mTmpTransform = mat4(1);

    updateFBOSize();
    addGrid();
}

void ZLineChart::onWindowChange(int width, int height) {
    ZView::onWindowChange(width, height);

    updateFBOSize();
    draw();
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

    vector<float> verts;
    vector<int> edges;

    for (int lineIndex = 0; lineIndex < mLineCount; lineIndex++) {
        for (uint i = 0; i < mResolution; i++) {
            float x = mix(mXBound.x, mXBound.y, (float) i / mResolution);
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

    mat4 projection = ortho(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 10.0f) * mTmpTransform;
    mShader->setMat4("uVPMatrix", projection);

    glViewport(0, 0, getWidth(), getHeight());

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

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

    mShader->setVec4("uColor", vec4(1.0, 0.0, 0.0, 1.0));

    // draw grid
    glBindBuffer(GL_ARRAY_BUFFER, mGridVertBuffer);
    glEnableVertexAttribArray(glGetAttribLocation(mShader->mID, "vPosUi"));
    glVertexAttribPointer(glGetAttribLocation(mShader->mID, "vPosUi"), 4, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 4, (void*) 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGridEdgeBuffer);
    glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, nullptr);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFinalFBO);
    glBlitFramebuffer(0, 0, getWidth(), getHeight(), 0, 0, getWidth(), getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    ZView::draw();
}

mat<4, 4, float> ZLineChart::getMatrix() const {
    return mTmpTransform;
}

void ZLineChart::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state) {
    ZView::onMouseDrag(absolute,start, delta, state);

    vec4 p1 = vec4(0,0,0,1);
    vec4 p2 = vec4(0.5,0.5,0,1);
    vec4 aScale = ((mTmpTransform * p2) - (mTmpTransform * p1)) / vec4(1,-1,0,0);
    vec4 aTranslation = (mTmpTransform * p1);

    cout << aTranslation.x << " " << aTranslation.y << " scale:" << aScale.x << " " << aScale.y << endl;
    if (state == mouseDown) {
        mLastDistance = absolute - getCenter();
        mLastMouse = absolute;
    }

    if (middleMouseIsDown()) {
        if (state == mouseDrag) {
            vec2 d = mLastMouse - absolute;
            mTmpTransform = translate(mat4(1), vec3(2.0) * vec3((d.x / -getWidth()), (d.y / getHeight()), 0)) * mTmpTransform;
        }

        mLastMouse = absolute;
    } else if (rightMouseIsDown()) {
        if (state == mouseDrag) {
            vec3 origin = vec3(0,0, 0);

            vec2 pos = (absolute - getCenter());
            //cout << d.x << " " << d.y << endl;

            vec2 percent = pos / mLastDistance;
            //mTmpTransform = scale(mat4(1), vec3(percent.x, percent.y, 1)) * mTmpTransform;
            mTmpTransform = (translate(mat4(1), origin) * (scale(mat4(1), vec3(percent.x, percent.y, 1)) * mTmpTransform) * translate(mat4(1), -origin));
            mLastDistance = pos;

        }


    }


    invalidate();
}
