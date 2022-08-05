//
// Created by lukas on 3/21/22.
//

#ifndef ZPATH_ZMATHNODEEDITOR_H
#define ZPATH_ZMATHNODEEDITOR_H

#include "ui/znodeeditor.h"

class ZMathNodeEditor : public ZNodeEditor {
public:
    ZMathNodeEditor(float maxWidth, float maxHeight, ZView *parent);

    void onFileDrop(int count, const char **paths);
};


#endif //ZPATH_ZMATHNODEEDITOR_H
