
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
    void updateLine(int index, float *points, int size);
    void setSize(int width, int height);
    float getMin();
    float getMax();
    void setMax(float max);
    void setMin(float min);

    int getLineCount();

    void addLine(vector<float> points);

    void updateLine(int index, vector<float> points);

    void resetZoom(vector<float> &points);
    void setZPosition(float zPosition);
private:
    unsigned int mFBO = -1;
    unsigned int mFinalFBO = -1;
    unsigned int mTexBuffer = -1;
    unsigned int mFinalTexBuffer = -1;
    unsigned int mRBO = -1;
    unsigned int mFinalRBO = -1;
    float mLineWidth = 2.0f;

    vector<unsigned int> mPoints;
    vector<unsigned int> mEdges;
    vector<unsigned int> mPointCount;
    ZShader* mShader;

    int mWidth = 600;
    int mHeight = 400;

    float mZPosition = 0.0;

    float mMin = INT_MAX;
    float mMax = INT_MIN;

    unsigned int mGridVertBuffer;
    unsigned int mGridEdgeBuffer;

    // Shader code
    const string ui_vs =
#include "shaders/chart.vs"
    ;

    const string ui_fs =
#include "shaders/chart.fs"
    ;


    void updateBuffers();

    void addGrid();


};


#endif //ZPATH_ZCHARTRENDERER_H
