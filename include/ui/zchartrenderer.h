
#ifndef ZPATH_ZCHARTRENDERER_H
#define ZPATH_ZCHARTRENDERER_H

#include <string>
#include <vector>
#include "glad/glad.h"
#include "ui/zshader.h"

using namespace std;

class ZChartRenderer {

public:
    ZChartRenderer(int width, int height);
    void onDraw();
    unsigned int getTexID();
    unsigned int getFrameID();

    void addLine(float points[], int size);
    void updateLine(int index, float points[]);
    void setSize(int width, int height);

private:
    unsigned int mFBO;
    unsigned int mFinalFBO;
    unsigned int mTexBuffer;
    unsigned int mFinalTexBuffer;
    unsigned int mRBO;
    unsigned int mFinalRBO;
    float mLineWidth = 2.0f;

    vector<unsigned int> mPoints;
    vector<unsigned int> mEdges;
    vector<unsigned int> mPointCount;
    ZShader* mShader;

    int mWidth = 600;
    int mHeight = 400;

    float mMin = INT_MAX;
    float mMax = INT_MIN;

    // Shader code
    const string ui_vs =
#include "shaders/chart.vs"
    ;

    const string ui_fs =
#include "shaders/chart.fs"
    ;


    void updateBuffers();
};


#endif //ZPATH_ZCHARTRENDERER_H
