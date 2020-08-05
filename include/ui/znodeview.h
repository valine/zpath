//
// Created by lukas on 8/4/20.
//

#ifndef ZPATH_ZNODEVIEW_H
#define ZPATH_ZNODEVIEW_H


#include <ui/zview.h>

class ZNodeView : public ZView {
public:
    ZNodeView(float maxWidth, float maxHeight, ZView *parent);

    void addNode();
};


#endif //ZPATH_ZNODEVIEW_H
