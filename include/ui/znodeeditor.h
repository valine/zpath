//
// Created by lukas on 8/4/20.
//

#ifndef ZPATH_ZNODEEDITOR_H
#define ZPATH_ZNODEEDITOR_H


static const int NO_SELECTION = -1;

#include <ui/zview.h>
#include "nodeview.h"

class ZNodeEditor : public ZView {
public:
    ZNodeEditor(float maxWidth, float maxHeight, ZView *parent);

    void addNode();

    void onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state) override;


private:
    vector<NodeView*> mNodeViews;
    vec2 mInitialOffset;
    int mDragNode = 0;

};


#endif //ZPATH_ZNODEEDITOR_H
