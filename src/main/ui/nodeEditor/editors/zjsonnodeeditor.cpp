//
// Created by lukas on 3/21/22.
//

#include "ui/zjsonnodeeditor.h"
#include "ui/znodedefstore.h"
#include "utils/datastore.h"
#include "utils/znodeutil.h"


ZJsonNodeEditor::ZJsonNodeEditor(float maxWidth, float maxHeight, ZView *parent) :
    ZNodeEditor(maxWidth, maxHeight, parent) {
    auto nodeTypes = ZNodeDefStore::get().getJsonNodeTypes();
    setNodeTypes(nodeTypes);
    setExpVisibility(false);
}

void ZJsonNodeEditor::onFileDrop(int count, const char** paths) {
    string path(paths[0]);
    string ext = getFileExtension(path);

    if (ext == "json") {
        cout << "Data importing..." << endl;
        json j = DataStore::get().parseJsonFromFile(path);

        ZNodeView* root = ZNodeUtil::get().nodesFromJson(j, nullptr);
        addNodeGraph(root, vec2(10), 0);

    } else {
        cout << "File type: " << ext << " not supported" << endl;
    }
}
