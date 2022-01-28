#include <utility>

//
// Created by lukas on 12/5/20.
//

#include "ui/zlinechart.h"

ZLineChart::ZLineChart(float width, float height, ZView *parent) : ZView(width, height, parent) {

}

void ZLineChart::onCreate() {
    ZView::onCreate();
    initView();

}

void ZLineChart::initView() {
    if (!mViewInit) {
        mViewInit = true;

        mShader = new ZShader(ui_vs, ui_fs);
        mHeatShader = new ZShader(heat_vs, heat_fs);

        glGenTextures(1, &mFinalTexBuffer);
        glGenFramebuffers(1, &mFinalFBO);
        glGenRenderbuffers(1, &mFinalRBO);

        glGenBuffers(1, &mHeatVertBuffer);
        glGenBuffers(1, &mHeatEdgeBuffer);
        glGenTextures(1, &mHeatTexBuffer);

        glGenBuffers(1, &mBGridEdgeBuffer);
        glGenBuffers(1, &mBGridVertBuffer);

        glGenVertexArrays(1, &mHeatVAO);
        glBindVertexArray(mHeatVAO);

        glGenBuffers(1, &mGridVertBuffer);
        glGenBuffers(1, &mGridEdgeBuffer);

        // Heat map vao
        glBindBuffer(GL_ARRAY_BUFFER, mHeatVertBuffer);
        int dimension = 4;
        glEnableVertexAttribArray(glGetAttribLocation(mHeatShader->mID, "vPosUi"));
        glVertexAttribPointer(glGetAttribLocation(mHeatShader->mID, "vPosUi"), dimension, GL_FLOAT, GL_FALSE,
                              sizeof(float) * dimension, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHeatEdgeBuffer);
        glBindVertexArray(0);

        // Line vao
        glGenVertexArrays(1, &mVAO);
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mBGridVertBuffer);
        glEnableVertexAttribArray(glGetAttribLocation(mShader->mID, "vPosUi"));
        glVertexAttribPointer(glGetAttribLocation(mShader->mID, "vPosUi"), 4, GL_FLOAT, GL_FALSE,
                              sizeof(float) * 4, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBGridEdgeBuffer);
        glBindVertexArray(0);

        glGenVertexArrays(1, &mGridVAO);
        glBindVertexArray(mGridVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mGridVertBuffer);
        glEnableVertexAttribArray(glGetAttribLocation(mShader->mID, "vPosUi"));
        glVertexAttribPointer(glGetAttribLocation(mShader->mID, "vPosUi"), 4, GL_FLOAT, GL_FALSE,
                              sizeof(float) * 4, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGridEdgeBuffer);
        glBindVertexArray(0);

        initHeatLUT();

        mBackground = new ZTexture(mFinalTexBuffer);
        setBackgroundImage(mBackground);

        mTmpTransform = ortho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 10.0f);
        mTransform = mDefaultMat;
        mTransformCurrent = true;

        computeChartBounds();
        updateFBOSize();
        initGrid();
        initBackgroundGrid();

        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void ZLineChart::setDefaultMat(vec4 defaultBounds) {
    mDefaultMat = ortho(defaultBounds.x, defaultBounds.y,
                        defaultBounds.w, defaultBounds.z, -1.0f, 10.0f);
}

void ZLineChart::initHeatLUT()  {
    glGenTextures(1, &mHeatLUTBuffer);
    mHeatShader->use();
    glUniform1i(glGetUniformLocation(mHeatShader->mID, "textureMap"), 0);
    glUniform1i(glGetUniformLocation(mHeatShader->mID, "texLut"), 1);
    int res = 15;
    vector<float> pixels = {
            0.0f,0.0f,0.0f,
            0.000000f,0.000000f,0.520996f,
            0.000000f,0.000000f,1.000000f,
            0.000000f,0.051269f,1.000000f,
            0.000000f,0.215861f,1.000000f,
            0.000000f,0.520996f,1.000000f,
            0.051269f,1.000000f,1.000000f,
            0.215861f,1.000000f,0.520996f,
            0.520996f,1.000000f,0.215861f,
            1.000000f,1.000000f,0.051269f,
            1.000000f,1.000000f,0.000000f,
            1.000000f,0.520996f,0.000000f,
            1.000000f,0.215861f,0.000000f,
            1.000000f,0.051269f,0.000000f,
            0.215861f,0.000000f,0.000000f,
    };

    glBindTexture(GL_TEXTURE_2D, mHeatLUTBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, res, 1, 0, GL_RGB, GL_FLOAT, &pixels[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void ZLineChart::updateHeatMap() {

    if (mPointListener == nullptr) {
        return;
    }
                        // pos,  texture
    vector<float> verts = {mXBound.x, mYBound.x, 0, 0,
                           mXBound.y, mYBound.x, 1, 0,
                           mXBound.x, mYBound.y, 0, 1,
                           mXBound.y, mYBound.y, 1, 1};
    vector<int> edges = {0,2,1, 1,2,3};

    glBindBuffer(GL_ARRAY_BUFFER, mHeatVertBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHeatEdgeBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(int), &edges[0], GL_DYNAMIC_DRAW);

    vector<float> pixels = mPointListener({(int) 0, (int) 0}, 0);

    glBindTexture(GL_TEXTURE_2D, mHeatTexBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, std::min(mResolution, mMaxResolution),
                 std::min(mResolution, mMaxResolution), 0, GL_RED, GL_FLOAT, &pixels[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mHeatInitialized = true;
}

void ZLineChart::updateData() {
    // Run from UI thread
    if (mPointListener == nullptr) {
        return;
    }
    if (mInputType == LINE || mInputType == LINE_2X) {
        updateChart1D();
    } else if (mInputType == LINE_2D) {
        updateChart2D();
    } else if (mInputType == HEAT_MAP){
        updateHeatMap();
    }

    resetTmpTransform();

    invalidate();
}

void ZLineChart::resetTmpTransform()  {
    // Reset the tmp transform. The tmp transform is used while evaluation
    // happens on a background thread. This ensures the ui will not lag
    // when evaluating large graphs.
    mTmpTransform = ortho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 10.0f);
    mTransformCurrent = true;
}

bool ZLineChart::isTransformCurrent() {
    return mTransformCurrent;
}

void ZLineChart::initBackgroundGrid() {
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

    glBindBuffer(GL_ARRAY_BUFFER, mBGridVertBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBGridEdgeBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(int), &edges[0], GL_DYNAMIC_DRAW);
}

void ZLineChart::initGrid() {
    vector<float> verts = {-1e7, 0, 0, 0,
                           1e7, 0, 0, 0,
                           0, -1e7, 0, 0,
                           0, 1e7, 0, 0};

    vector<int> edges = {0, 1, 2, 3};
    glBindBuffer(GL_ARRAY_BUFFER, mGridVertBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGridEdgeBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(int), &edges[0], GL_DYNAMIC_DRAW);
}

void ZLineChart::onWindowChange(int width, int height) {
    bool needsUpdate = false;
    if (width != getWidth() || height != getHeight()) {
        needsUpdate = true;
    }
    ZView::onWindowChange(width, height);

    if (needsUpdate) {
        updateFBOSize();
    }
}

void ZLineChart::updateFBOSize() {
    int width = getWidth();
    int height = getHeight();

    glEnable(GL_MULTISAMPLE);

    glBindFramebuffer(GL_FRAMEBUFFER, mFinalFBO);
    glBindTexture(GL_TEXTURE_2D, mFinalTexBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindRenderbuffer(GL_RENDERBUFFER, mFinalRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFinalTexBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mFinalTexBuffer);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    computeChartBounds();
}

void ZLineChart::updateChart2D() {
    vector<float> verts;
    for (int lineIndex = 0; lineIndex < mLineCount; lineIndex++) {
        vector<float> output;
        for (uint i = 0; i < mResolution; i++) {
            output = mPointListener({(int) i}, lineIndex);
            verts.push_back(output.at(0));
            verts.push_back(output.at(1));
            verts.push_back(0);
            verts.push_back(0);
        }

        // Initialize buffers
        if (mPoints.size() <= lineIndex) {
            unsigned int vao;
            glGenVertexArrays(1, &vao);
            mLineVAO.push_back(vao);


            mPointCount.push_back(verts.size());
            unsigned int lineBuffer;
            glGenBuffers(1, &lineBuffer);
            mPoints.push_back(lineBuffer);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
            glEnableVertexAttribArray(glGetAttribLocation(mShader->mID, "vPosUi"));
            glVertexAttribPointer(glGetAttribLocation(mShader->mID, "vPosUi"), 4, GL_FLOAT, GL_FALSE,
                                  sizeof(float) * 4, nullptr);
            glBindVertexArray(0);
        } else {
            mPointCount.at(lineIndex) = verts.size();
        }

        glBindBuffer(GL_ARRAY_BUFFER, mPoints.at(lineIndex));
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);
    }
}

void ZLineChart::updateChart1D() {
    for (int lineIndex = 0; lineIndex < mLineCount; lineIndex++) {
        vector<float> verts;
        for (uint i = 0; i < mResolution; i++) {
            vector<float> y = mPointListener({(int) i}, lineIndex);
            verts.push_back(((float) i / (float) (mResolution - 1)));
            verts.push_back(y.at(0));
            verts.push_back(0);
            verts.push_back(0);
        }

        // Initialize buffers
        if (mPoints.size() <= lineIndex) {

            unsigned int vao;
            glGenVertexArrays(1, &vao);
            mLineVAO.push_back(vao);

            mPointCount.push_back(verts.size());
            unsigned int lineBuffer;
            glGenBuffers(1, &lineBuffer);
            mPoints.push_back(lineBuffer);


            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
            glEnableVertexAttribArray(glGetAttribLocation(mShader->mID, "vPosUi"));
            glVertexAttribPointer(glGetAttribLocation(mShader->mID, "vPosUi"), 4, GL_FLOAT, GL_FALSE,
                                  sizeof(float) * 4, nullptr);
            glBindVertexArray(0);

        } else {
            mPointCount.at(lineIndex) = verts.size();
        }

        glBindBuffer(GL_ARRAY_BUFFER, mPoints.at(lineIndex));
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);
    }
}

void ZLineChart::draw() {
    if (mGpuMode) {
        glBindFramebuffer(GL_FRAMEBUFFER, mFinalFBO);
        glBindTexture(GL_TEXTURE_2D, mFinalTexBuffer);
        glViewport(0, 0, getWidth(), getHeight());
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Todo: Retrieve constant parameters from node and pass to shader as uniform
    } else {
        drawCpu();
    }

    ZView::draw();
}

void ZLineChart::drawCpu() {
    initView();
    if (mDataInvalid) {
        mDataInvalid = false;
        updateData();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mFinalFBO);
    glBindTexture(GL_TEXTURE_2D, mFinalTexBuffer);
    glViewport(0, 0, getWidth(), getHeight());
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw heat map. Heat map update is triggered by a second thread,
// so check that everything is initialized before drawing.
    if (mInputType == HEAT_MAP && mHeatInitialized) {
        mHeatShader->use();
        mHeatShader->setMat4("uVPMatrix", mTransform);

        mHeatShader->setFloat("zmin", mZBound.x);
        mHeatShader->setFloat("zmax", mZBound.y);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mHeatTexBuffer);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mHeatLUTBuffer);

        mHeatShader->setVec4("uColor", green.get(mColorMode));

        int triangles = 2;

        glBindVertexArray(mHeatVAO);
        glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    mShader->use();
    // mat4 projection = mTmpTransform;
    mShader->setMat4("uVPMatrix", mTransform);


    if (mInputType != HEAT_MAP) {
        // draw background grid
        mShader->setVec4("uColor", grey.get(mColorMode));

        glLineWidth(2.0);

        glBindVertexArray(mVAO);
        glDrawElements(GL_LINES, mBGridVCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    // draw grid
    mShader->setVec4("uColor", red.get(mColorMode));

    glBindVertexArray(mGridVAO);
    glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // mat4 projection = mTmpTransform;
    mShader->setMat4("uVPMatrix", mTmpTransform);

    if (mInputType != HEAT_MAP) {
        // Draw graph lines
        glLineWidth(2.0);
        for (int i = mPoints.size() - 1; i >= 0; i--) {
            mShader->setVec4("uColor", vec4(1.0, 0.0, 0.0, 1.0) *
                                       vec4(vec3((float) i / mPoints.size()), 1.0));
            glBindVertexArray(mLineVAO.at(i));
            glDrawArrays(GL_LINE_STRIP, 0, (mPointCount.at(i) / 4));
        }
    }

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ZLineChart::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) {
    ZView::onMouseDrag(absolute, start, delta, state, 0);

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

    if ((middleMouseIsDown() || mouseIsDown()) && shiftKeyPressed()) {
        if (state == mouseDrag) {
            vec2 d = (mLastMouse - positionInView) * vec2(2.0);
            mTmpTransform = translate(mat4(1),
                                      vec3(d.x / -getWidth(), d.y / -getHeight(), 0)) * mTmpTransform;
            mTransform = translate(mat4(1),
                                   vec3(d.x / -getWidth(), d.y / -getHeight(), 0)) * mTransform;
            mTransformCurrent = false;
            needsRefresh = true;
        }

        mLastMouse = positionInView;
    } else if (rightMouseIsDown() && shiftKeyPressed()) {
        vec2 minPos = vec2(1,1);

        vec4 p1 = vec4(0,0,0,1);
        vec4 p2 = vec4(0.5,0.5,0,1);
        vec4 sampleScale = ((mTransform * p2) - (mTransform * p1)) / vec4(1,-1,0,0);

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
                if (sampleScale.x == 0) {
                    mTransform = mat4(1);
                    resetZoom();
                    return;
                }

                percent.x = 0.01 / sampleScale.x;
            } else {
                mLastMouse.x = positionInView.x;
            }

            if (positionInView.y < minPos.y) {
                if (sampleScale.y == 0) {
                    resetZoom();
                    return;
                }
                percent.y = 0.01 / sampleScale.y;
            } else {
                mLastMouse.y = positionInView.y;
            }
            mTmpTransform = (translate(mat4(1), mScaleOrigin) *
                             scale(mat4(1), vec3(percent.x, percent.y, 1)) *
                             translate(mat4(1), -mScaleOrigin)) * mTmpTransform;

            mTransform = (translate(mat4(1), mScaleOrigin) *
                             scale(mat4(1), vec3(percent.x, percent.y, 1)) *
                             translate(mat4(1), -mScaleOrigin)) * mTransform;

            mTransformCurrent = false;
            needsRefresh = true;
        }
    }

    if (state == mouseUp) {
        needsRefresh = true;
    }

    if (needsRefresh && mInvalidateListener != nullptr) {
        mInvalidateListener();
    }
}

void ZLineChart::resetZoom() {
    mTmpTransform = ortho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 10.0f);
    mTransform = mDefaultMat;
    mTransformCurrent = true;
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
        mTmpTransform = (translate(mat4(1), origin) *
                         scale(mat4(1), vec3(percent.x, percent.y, 1)) *
                         translate(mat4(1), -origin)) * mTmpTransform;
        mTransform = (translate(mat4(1), origin) *
                         scale(mat4(1), vec3(percent.x, percent.y, 1)) *
                         translate(mat4(1), -origin)) * mTransform;
        mTransformCurrent = false;
        if (mInvalidateListener != nullptr) {
            mInvalidateListener();
        }
    }
}
