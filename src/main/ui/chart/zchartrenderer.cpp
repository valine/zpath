
#include <iostream>
#include <ui/zshader.h>
#include <utils/zimageutil.h>
#include "ui/zchartrenderer.h"
#include "png.h"

ZChartRenderer::ZChartRenderer(int width, int height) {

    mShader = new ZShader(ui_vs, ui_fs);
    mWidth = width;
    mHeight = height;
    updateBuffers();

    glGenFramebuffers(1, &mFBO);
    glGenTextures(1, &mTexBuffer);
    glGenRenderbuffers(1, &mRBO);


    glGenTextures(1, &mFinalTexBuffer);
    glGenFramebuffers(1, &mFinalFBO);
    glGenRenderbuffers(1, &mFinalRBO);

    addGrid();
}

void ZChartRenderer::updateBuffers() {

    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mTexBuffer);

    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA, mWidth, mHeight, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH_COMPONENT, mWidth, mHeight);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mTexBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mTexBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, mFinalFBO);
    glBindTexture(GL_TEXTURE_2D, mFinalTexBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBindRenderbuffer(GL_RENDERBUFFER, mFinalRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWidth, mHeight);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFinalTexBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mFinalTexBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ZChartRenderer::onDraw() {
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    mShader->use();

    mat4 projection = ortho(0.0f, (float) 1.0, (float) mMax, mMin, -10.0f, 100.0f);
    mShader->setMat4("uVPMatrix", projection);

    glViewport(0, 0, mWidth, mHeight);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


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
    glDrawElements(GL_LINES, mGridEdgeBuffer, GL_UNSIGNED_INT, nullptr);


    glDepthMask(true);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFinalFBO);
    glBlitFramebuffer(0, 0, mWidth, mHeight, 0, 0, mWidth, mHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int ZChartRenderer::getTexID() {
    return mFinalTexBuffer;
}

unsigned int ZChartRenderer::getFrameID() {
    return mFinalFBO;
}


void ZChartRenderer::resetZoom(vector<float> &points) {
    for (uint i = 0; i < points.size(); i++) {
        if (points.at(i) > mMax) {
            mMax = points.at(i);
        }

        if (points.at(i) < mMin) {
            mMin = points.at(i);
        }
    }
}

void ZChartRenderer::addLine(float *points, int size) {

    vector<float> verts;
    vector<int> edges;

    for (uint i = 0; i < size; i++) {
        if (points[i] > mMax) {
            mMax = points[i];
        }

        if (points[i] < mMin) {
            mMin = points[i];
        }
    }

    for (uint i = 0; i < size; i++) {
        verts.push_back(((float) i / (float) (size - 1)));
        verts.push_back(points[i]);
        verts.push_back(0);
        verts.push_back(0);

        if (i != size - 1) {
            edges.push_back(i);
            edges.push_back(i + 1);
        }
    }

    mPointCount.push_back(edges.size());
    unsigned int lineBuffer;
    glGenBuffers(1, &lineBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);
    mPoints.push_back(lineBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int edgeBuffer;
    glGenBuffers(1, &edgeBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(int), &edges[0], GL_DYNAMIC_DRAW);
    mEdges.push_back(edgeBuffer);
}

void ZChartRenderer::updateLine(int index, float *points, int size) {
    if (mPoints.size() <= index) {
        for (int i = mPoints.size(); i < index; i++) {
            addLine(new float[1], 0);
        }
        addLine(points, size);
        return;
    }

    vector<float> verts;
    vector<int> edges;

    for (uint i = 0; i < size; i++) {
        if (points[i] > mMax) {
            mMax = points[i];
        }

        if (points[i] < mMin) {
            mMin = points[i];
        }
    }

    for (uint i = 0; i < size; i++) {
        verts.push_back(((float) i / (float) (size - 1)));
        verts.push_back(points[i]);
        verts.push_back(0);
        verts.push_back(0);

        if (i != size - 1) {
            edges.push_back(i);
            edges.push_back(i + 1);
        }
    }

    mPointCount.at(index) = edges.size();
    glBindBuffer(GL_ARRAY_BUFFER, mPoints.at(index));
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  mEdges.at(index));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(int), &edges[0], GL_DYNAMIC_DRAW);

    updateBuffers();
}

void ZChartRenderer::addLine(vector<float> points) {

    vector<float> verts;
    vector<int> edges;

    resetZoom(points);

    for (uint i = 0; i <  points.size(); i++) {
        verts.push_back(((float) i / (float) ( points.size() - 1)));
        verts.push_back(points.at(i));
        verts.push_back(0);
        verts.push_back(0);

        if (i !=  points.size() - 1) {
            edges.push_back(i);
            edges.push_back(i + 1);
        }
    }

    mPointCount.push_back(edges.size());
    unsigned int lineBuffer;
    glGenBuffers(1, &lineBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);
    mPoints.push_back(lineBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int edgeBuffer;
    glGenBuffers(1, &edgeBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(int), &edges[0], GL_DYNAMIC_DRAW);
    mEdges.push_back(edgeBuffer);
}

void ZChartRenderer::addGrid() {
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

void ZChartRenderer::updateLine(int index, vector<float> points) {
    vector<float> verts;
    vector<int> edges;

    if (mPoints.size() <= index) {
        for (int i = mPoints.size(); i < index; i++) {
            addLine(vector<float>(1,0));
        }
        addLine(points);
        return;
    }

    for (float point : points) {
        if (point > mMax) {
            mMax = point;
        }

        if (point < mMin) {
            mMin = point;
        }
    }

    for (uint i = 0; i < points.size(); i++) {
        verts.push_back(((float) i / (float) (points.size() - 1)));
        verts.push_back(points.at(i));
        verts.push_back(0);
        verts.push_back(0);

        if (i != points.size() - 1) {
            edges.push_back(i);
            edges.push_back(i + 1);
        }
    }

    mPointCount.at(index) = edges.size();
    glBindBuffer(GL_ARRAY_BUFFER, mPoints.at(index));
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  mEdges.at(index));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(int), &edges[0], GL_DYNAMIC_DRAW);

    updateBuffers();
}

void ZChartRenderer::setSize(int width, int height) {
    mWidth = width;
    mHeight = height;
    updateBuffers();
    onDraw();
}

float ZChartRenderer::getMin() {
    return mMin;
}

float ZChartRenderer::getMax() {
    return mMax;
}

void ZChartRenderer::setMax(float max) {
    mMax = max;

}

void ZChartRenderer::setMin(float min) {
    mMin = min;
}

int ZChartRenderer::getLineCount() {
    return mPoints.size();
}
