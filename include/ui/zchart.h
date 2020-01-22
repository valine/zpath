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

private:

    ZChartRenderer* mRenderer;
    ZTexture* mBackground;

};


#endif //ZPATH_ZCHART_H
