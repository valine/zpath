//
// Created by lukas on 3/21/22.
//

#include "ui/zmathnodeeditor.h"
#include "ui/nodetype.h"
#include "ui/zmathnodedef.h"

ZMathNodeEditor::ZMathNodeEditor(float maxWidth, float maxHeight, ZView *parent) :
    ZNodeEditor(maxWidth, maxHeight, parent) {

    auto nodeTypes = ZMathNodeDef::get().getNodeTypes();

    setNodeTypes(nodeTypes);

}
