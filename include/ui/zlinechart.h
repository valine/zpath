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

    void setChartBounds(vec2 x, vec2 y) {
        mXBound = x;
        mYBound = y;
        updateLineBuffers();
    }

    vec2 getXBounds() {
        return mXBound;
    }

    vec2 getYBounds() {
        return mYBound;
    }

    void setLineCount(int count) {
        mLineCount = count;
        updateLineBuffers();
    }

    void setResolution(int resolution) {
        mResolution = resolution;
    }

    void updateChart() {

        if (mListener != nullptr) {
            for (int i = 0; i < mResolution; i++) {
                int lineIndex = 0;
                vector<float> y = mListener({i}, lineIndex); //  todo: replace 0 with line index
            }
        }
    }

    void onWindowChange(int width, int height) override;

private:

    function<vector<float>(vector<int>, int index)> mListener;

    // X is min, Y is max
    vec2 mXBound = vec2(0, 3);
    vec2 mYBound = vec2(0, 3);

    // Temp bound is set on ui thread, the real bounds get updated after evaluation
    vec2 mTempXBound = vec2(0);
    vec2 mTempYBound = vec2(0);

    // OpenGL buffers
    unsigned int mFBO = -1;
    unsigned int mFinalFBO = -1;
    unsigned int mTexBuffer = -1;
    unsigned int mFinalTexBuffer = -1;
    unsigned int mRBO = -1;
    unsigned int mFinalRBO = -1;

    unsigned int mGridVertBuffer = -1;
    unsigned int mGridEdgeBuffer = -1;

    vector<unsigned int> mPoints;
    vector<unsigned int> mEdges;
    vector<unsigned int> mPointCount;
    ZShader* mShader;

    ZTexture* mBackground;
    int mLineCount = 1;

    float mLineWidth = 2;

    // This will be set by view width
    int mResolution = 30;

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
};


#endif //ZPATH_ZLINECHART_H
