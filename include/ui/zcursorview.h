//
// Created by lukas on 12/21/20.
//

#ifndef ZPATH_ZCURSORVIEW_H
#define ZPATH_ZCURSORVIEW_H


#include "zview.h"

class ZCursorView : public ZView {
public:
    ZCursorView(ZView* parent);
    void setPosition(vec2 position);
private:
    ZLineView* mHorizontal;
    ZLineView* mVertical;
};


#endif //ZPATH_ZCURSORVIEW_H
