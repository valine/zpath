//
// Created by lukas on 7/27/20.
//

#ifndef ZPATH_ZLINEVIEW_H
#define ZPATH_ZLINEVIEW_H


#include "zview.h"


class ZLineView : public ZView {

public:

    ZLineView(float maxWidth, float maxHeight, ZView *parent);
    void init();
};


#endif //ZPATH_ZLINEVIEW_H
