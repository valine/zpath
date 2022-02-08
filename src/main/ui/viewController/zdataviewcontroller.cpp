//
// Created by lukas on 1/28/22.
//

#include "ui/zdataviewcontroller.h"
#include "utils/dataimportutil.h"
#include "ui/zlistview.h"

void ZDataViewController::onCreate() {
    ZViewController::onCreate();

    mListView = new ZListView(this);
    mListView->setMargin(10);
}

void ZDataViewController::onFileDrop(int count, const char** paths) {
    string path(paths[0]);
    string ext = getFileExtension(path);

    if (ext == "json") {
        cout << "Data importing..." << endl;
        json j = DataStore::parseJsonFromFile(path);
        DataStore::storeJson(j);
        mListView->addItem(path);
    } else {
        cout << "File type: " << ext << " not supported" << endl;
    }
}

string ZDataViewController::getFileExtension(string path) {
    return path.substr(path.find_last_of(".") + 1) ;
}

