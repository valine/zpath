//
// Created by Lukas Valine on 5/19/19.
//

#ifndef ZPATH_ZCHART_H
#define ZPATH_ZCHART_H
#include "zview.h"
#include "zchartrenderer.h"

class ZChart : public ZView {

public:
    ZChart(float maxWidth, float maxHeight, ZView *parent);
    void draw();
    void addLine(float points[], int size);
    void updateLine(int index, float points[], int size);
    void onWindowChange(int width, int height) override;
    void setMinMax(float, float);


    void resetZoom();
    void addLine(vector<float>);
    void updateLine(int index, vector<float>);
private:

    vector<bool> mLinesInvalid;
    vector<vector<float>> mPendingLines;

    ZChartRenderer* mRenderer = nullptr;
    ZTexture* mBackground = nullptr;

};


#endif //ZPATH_ZCHART_H
