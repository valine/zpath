#include <utility>

#include <utility>

//
// Created by lukas on 12/5/20.
//

#ifndef ZPATH_ZLINECHART_H
#define ZPATH_ZLINECHART_H


static const int LINE = 0;
static const int LINE_2X = 1;
static const int LINE_2DD = 2;
static const int HEAT_MAP = 3;
static const int RGB = 5;

#include "zview.h"
#include <regex>

class ZLineChart : public ZView {

public:
    ZLineChart(float width, float height, ZView* parent);
    void draw() override;

    // This might not be needed
    void setChartListener(function<vector<float>(vector<int>, int index)> listener){
        mPointListener = move(listener);
    }

    // This might not be needed
    void setShaderListener(function<ZShader()> listener){
        mShaderListener = move(listener);
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

    void setZBound(vec2 zbound) {
        mZBound = zbound;
    }

    vec2 getZBound() {
        return mZBound;
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

    int getMaxResolution() {
        return mMaxResolution;
    }

    void setMaxResolution(int res) {
        mMaxResolution = res;
    }

    void resetZoom();
    void resetTmpTransform();
    bool isTransformCurrent();

    void setDefaultMat(vec4 defaultBounds);

    /**
     * False to use cpu rendering
     * @param mode Gpu mode
     */
    void setGpuMode(bool mode) {
        mGpuMode = mode;
    }
private:
    bool mGpuMode = false;
    function<void()> mInvalidateListener;

    // Retrieve individual points from view parent. Used for CPU rendering
    function<vector<float>(vector<int>, int index)> mPointListener = nullptr;

    // Retrieve shader from view parent. Used for GPU rendering
    function<ZShader()> mShaderListener = nullptr;

    bool mViewInit = false;
    float defaultRange = 5.0;
    mat4 mDefaultMat = ortho(-defaultRange, defaultRange, defaultRange, -defaultRange, -1.0f, 10.0f);

    // Temp bound is set on ui thread, the real bounds get updated after evaluation
    vec2 mLastMouse = vec2(0);
    mat4 mTmpTransform;
    bool mTransformCurrent = true;
    mat4 mTmpTransformIdentity = mat4(1);
    mat4 mTransform;
    vec3 mScaleOrigin = vec3(0);
    vec2 mXBound = vec2(0);
    vec2 mYBound = vec2(0);
    vec2 mZBound = vec2(-1,1);

    // OpenGL buffers
    unsigned int mFinalFBO = -1;
    unsigned int mFinalTexBuffer = -1;
    unsigned int mFinalRBO = -1;

    unsigned int mHeatVAO;
    unsigned int mVAO;
    unsigned int mGridVAO;
    vector<unsigned int> mLineVAO;

    unsigned int mHeatLUTBuffer = -1;

    unsigned int mGridVertBuffer = -1;
    unsigned int mGridEdgeBuffer = -1;

    unsigned int mBGridVertBuffer = -1;
    unsigned int mBGridEdgeBuffer = -1;

    unsigned int mHeatTexBuffer = -1;
    unsigned int mHeatVertBuffer = -1;
    unsigned int mHeatEdgeBuffer = -1;


    int mBGridVCount = 0;

    vector<unsigned int> mPoints;
    vector<unsigned int> mPointCount;
    ZShader* mShader;
    ZShader* mHeatShader;
    ZShader* mRgbShader;

    ZTexture* mBackground;
    int mLineCount = 1;
    bool mDataInvalid = false;
    bool mHeatInitialized = false;

    float mLineWidth = 2;

    // This will be set by view width
    int mResolution = 2;

    int mMaxResolution = 60;

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

    const string rgb_fs =
        #include "shaders/rgb.fs"
    ;

    void updateFBOSize();
    void updateData();
    void initGrid();

    void initBackgroundGrid();

    void updateChart2D();
    void updateChart1D();

    void updateHeatMap();
    void updateRgb();

    void initHeatLUT();

    void initView();

    void onCreate();

    void drawCpu();
};


#endif //ZPATH_ZLINECHART_H
