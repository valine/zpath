#include <utility>

#include <utility>

//
// Created by lukas on 12/5/20.
//

#ifndef ZPATH_ZLINECHART_H
#define ZPATH_ZLINECHART_H


static const int LINE = 0;

static const int LINE_2X = 1;

static const int LINE_2D = 2;

static const int HEAT_MAP = 3;

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
    }

    void setResolution(int resolution) {
        mResolution = std::max(2, std::min(3000, resolution));
    }

    void setInputType(int input) {
        mInputType = input;
    }

    int getInputCount() {
        return mInputType;
    }

    int getResolution() {
        return mResolution;
    }

    void onWindowChange(int width, int height) override;

    void onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) override;

    void onScrollChange(double x, double y) override;

    void invalidateData() {
        mDataInvalid = true;
    }

    void setInvalidateListener(function<void()> listener) {
        mInvalidateListener = std::move(listener);
    }

private:
    function<void()> mInvalidateListener;
    function<vector<float>(vector<int>, int index)> mListener = nullptr;

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

    unsigned int mHeatTexBuffer = -1;
    unsigned int mHeatVertBuffer = -1;
    unsigned int mHeatEdgeBuffer = -1;


    int mBGridVCount = 0;

    vector<unsigned int> mPoints;
    vector<unsigned int> mEdges;
    vector<unsigned int> mPointCount;
    ZShader* mShader;
    ZShader* mHeatShader;

    ZTexture* mBackground;
    int mLineCount = 1;
    bool mDataInvalid = false;

    float mLineWidth = 2;

    // This will be set by view width
    int mResolution = 2;

    // Input dimensions, default should be 1
    int mInputType = 1;

    // Shader code
    const string ui_vs =
        #include "shaders/chart.vs"
    ;

    const string ui_fs =
        #include "shaders/chart.fs"
    ;

    // Shader code
    const string heat_vs =
        #include "shaders/heatmap.vs"
    ;

    const string heat_fs =
        #include "shaders/heatmap.fs"
    ;

    void updateFBOSize();
    void updateData();
    void initGrid();

    void initBackgroundGrid();

    void updateChart2D();
    void updateChart1D();

    void updateHeatMap();

};


#endif //ZPATH_ZLINECHART_H
