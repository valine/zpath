//
// Created by lukas on 8/4/20.
//

#ifndef ZPATH_ZNODEVIEW_H
#define ZPATH_ZNODEVIEW_H


static const int NO_SELECTION = -1;

#include <ui/zview.h>

class ZNodeView : public ZView {
public:
    ZNodeView(float maxWidth, float maxHeight, ZView *parent);

    void addNode();

    void onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state) override;


private:
    vector<ZView*> mNodeViews;
    vec2 mInitialOffset;
    int mDragNode = 0;

};


#endif //ZPATH_ZNODEVIEW_H
