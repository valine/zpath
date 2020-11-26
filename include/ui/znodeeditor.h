//
// Created by lukas on 8/4/20.
//

#ifndef ZPATH_ZNODEEDITOR_H
#define ZPATH_ZNODEEDITOR_H


static const int NO_SELECTION = -1;
static const int SOCKET_DRAG_IN = -2;
static const int SOCKET_DRAG_OUT = -3;
static const int NODE_DRAG = -4;
#include <ui/zview.h>
#include "nodeview.h"

class ZNodeEditor : public ZView {
public:
    ZNodeEditor(float maxWidth, float maxHeight, ZView *parent);

    void addNode();

    void onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state) override;


private:
    vector<ZNodeView*> mNodeViews;
    ZLineView* mTmpLine;
    vec2 mInitialOffset;
    int mDragNode = 0;
    int mDragType = NO_SELECTION;
    int mDragSocket = NO_SELECTION;
    vector<ZLineView*> mLineBucket;

    bool isSocketDrag();

    int getMouseOverNode();
    int getMouseOverOutSocket(ZNodeView *node);
    int getMouseOverInSocket(ZNodeView *node);

    void updateLines();

    ZLineView * getLine(int index);
};


#endif //ZPATH_ZNODEEDITOR_H
