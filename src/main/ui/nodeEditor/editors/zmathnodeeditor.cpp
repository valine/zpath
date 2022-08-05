//
// Created by lukas on 3/21/22.
//

#include "ui/zmathnodeeditor.h"
#include "ui/nodetype.h"
#include "ui/znodedefstore.h"

ZMathNodeEditor::ZMathNodeEditor(float maxWidth, float maxHeight, ZView *parent) :
        ZNodeEditor(maxWidth, maxHeight, parent, "/math") {


    auto nodeTypes = ZNodeDefStore::get().getMathNodeTypes();
    setNodeTypes(nodeTypes);
}


void ZMathNodeEditor::onFileDrop(int count, const char** paths) {
    string path(paths[0]);

    string ext = ZUtil::getFileExtension(path);
    cout << "Data importing..." << endl;
    if (ext == "jpg" || ext == "png") {
        const char *c = path.c_str();

        ZUtil::Image img = ZUtil::loadTexture(c);
        DataStore::get().storeData(img, path);
        int index = DataStore::get().getFileList().size() - 1;
        auto node = addNode(ZNodeUtil::get().getNodeType("image"));
        node->mDropDown->selectItem(index);
        node->invalidateSingleNode();
    } else {
        cout << "File type: " << ext << " not supported" << endl;
    }
}


