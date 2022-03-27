//
// Created by lukas on 3/21/22.
//

#include "ui/zjsonnodeeditor.h"
#include "ui/znodedefstore.h"

ZJsonNodeEditor::ZJsonNodeEditor(float maxWidth, float maxHeight, ZView *parent) :
    ZNodeEditor(maxWidth, maxHeight, parent) {


    auto nodeTypes = ZNodeDefStore::get().getJsonNodeTypes();
    setNodeTypes(nodeTypes);

    setExpVisibility(false);
}
