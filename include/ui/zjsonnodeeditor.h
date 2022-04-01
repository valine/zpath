//
// Created by lukas on 3/21/22.
//

#ifndef ZPATH_ZJSONNODEEDITOR_H
#define ZPATH_ZJSONNODEEDITOR_H

#include "ui/znodeeditor.h"
class ZJsonNodeEditor : public ZNodeEditor {

public:
    ZJsonNodeEditor(float maxWidth, float maxHeight, ZView *parent);
    void onFileDrop(int count, const char **paths) override;
    string getFileExtension(string path) {
        return path.substr(path.find_last_of(".") + 1) ;
    }

private:
    void onLayoutFinished();
};


#endif //ZPATH_ZJSONNODEEDITOR_H
