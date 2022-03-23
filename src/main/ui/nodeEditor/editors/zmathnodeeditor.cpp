//
// Created by lukas on 3/21/22.
//

#include "ui/zmathnodeeditor.h"
#include "ui/nodetype.h"
#include "ui/znodedefstore.h"

ZMathNodeEditor::ZMathNodeEditor(float maxWidth, float maxHeight, ZView *parent) :
    ZNodeEditor(maxWidth, maxHeight, parent) {

    auto nodeTypes = ZNodeDefStore::get().getNodeTypes();

    setNodeTypes(nodeTypes);

}
