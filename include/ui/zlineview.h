//
// Created by lukas on 7/27/20.
//

#ifndef ZPATH_ZLINEVIEW_H
#define ZPATH_ZLINEVIEW_H


#include "zview.h"


class ZLineView : public ZView {

public:


    ZLineView(vec2 point1, vec2 point2, ZView *parent);

    void init();
    void computeBounds();
    void setPoints(vec2 point1, vec2 point2);
private:
    vec2 mPoint1;
    vec2 mPoint2;


};


#endif //ZPATH_ZLINEVIEW_H
