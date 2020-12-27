#include <utility>

#include <utility>

//
// Created by lukas on 12/5/20.
//

#ifndef ZPATH_ZLINECHART_H
#define ZPATH_ZLINECHART_H


#include "zview.h"
#include <regex>

class ZLineChart : public ZView {

public:
    ZLineChart(float width, float height, ZView* parent);
    void draw() override;

    // This might not be needed
    void setChartListener(function<vector<float>(vector<int>, int index)> listener){
        mListener = move(listener);
    }

    void computeChartBounds() {
        mat4 inverseMat = inverse(mTransform);
        vec4 min = (inverseMat * vec4(-1,0,0,1));
        vec4 max = (inverseMat * vec4(1,0,0,1));
        mXBound = vec2(min.x, max.x);

        vec4 minY = (inverseMat * vec4(0,1.0,0,1));
        vec4 maxY = (inverseMat * vec4(0,-1.0,0,1));
        mYBound = vec2(minY.y, maxY.y);
    }

    vec2 getXBounds() {
        return mXBound;
    }

    vec2 getYBounds() {
        return mYBound;
    }

    void setLineCount(int count) {
        mLineCount = count;
        //updateLineBuffers();
    }

    void setResolution(int resolution) {
        mResolution = std::max(2, std::min(3000, resolution));
    }

    void setInputCount(int input) {
        int maxInputCount = 2;
        mInputCount = std::min(input, maxInputCount);
        if(input > maxInputCount) {
            cout << "Clamping chart input count to " << maxInputCount << endl;
        }
    }

    int getInputCount() {
        return mInputCount;
    }

    int getResolution() {
        return mResolution;
    }

    void onWindowChange(int width, int height) override;

    void onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) override;

    void onScrollChange(double x, double y) override;

    void requestLineUpdate() {
        mLineUpdatedNeeded = true;
    }

    void setInvalidateListener(function<void()> listener) {
        mInvalidateListener = std::move(listener);
    }

private:
    function<void()> mInvalidateListener;
    function<vector<float>(vector<int>, int index)> mListener;

    mat4 mDefaultMat = ortho(-1.5f, 1.5f, 1.5f, -1.5f, -1.0f, 10.0f);

    // Temp bound is set on ui thread, the real bounds get updated after evaluation
    vec2 mLastMouse = vec2(0);
    mat4 mTmpTransform;
    mat4 mTmpTransformIdentity = mat4(1);
    mat4 mTransform;
    vec3 mScaleOrigin = vec3(0);
    vec2 mXBound = vec2(0);
    vec2 mYBound = vec2(0);

    // OpenGL buffers
    unsigned int mFBO = -1;
    unsigned int mFinalFBO = -1;
    unsigned int mTexBuffer = -1;
    unsigned int mFinalTexBuffer = -1;
    unsigned int mRBO = -1;
    unsigned int mFinalRBO = -1;

    unsigned int mGridVertBuffer = -1;
    unsigned int mGridEdgeBuffer = -1;

    unsigned int mBGridVertBuffer = -1;
    unsigned int mBGridEdgeBuffer = -1;
    int mBGridVCount = 0;

    vector<unsigned int> mPoints;
    vector<unsigned int> mEdges;
    vector<unsigned int> mPointCount;
    ZShader* mShader;

    ZTexture* mBackground;
    int mLineCount = 1;
    bool mLineUpdatedNeeded = false;

    float mLineWidth = 2;

    // This will be set by view width
    int mResolution = 2;

    // Input dimensions, default should be 1
    int mInputCount = 1;

    // Shader code
    const string ui_vs =
        #include "shaders/chart.vs"
    ;

    const string ui_fs =
        #include "shaders/chart.fs"
    ;

    void updateFBOSize();
    void updateLineBuffers();
    void addGrid();

    mat<4, 4, float> getMatrix() const;

    void addBackgroundGrid();

    void updateChart2D();
    void updateChart1D();
};


#endif //ZPATH_ZLINECHART_H
