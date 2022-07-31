//
// Created by lukas on 1/28/22.
//

#include <utils/znodestore.h>
#include "ui/zdataviewcontroller.h"
#include "utils/datastore.h"
#include "ui/zlistview.h"

void ZDataViewController::onCreate() {
    ZViewController::onCreate();
    mListView = new ZListView(this);
    mListView->setMargin(10);

    mListView->setCrumbInterface([](){
        vector<string> names = ZNodeStore::get().getProjectNames("/json");
        return names;
    });

    mListView->setOnCrumbChange([](int crumbIndex, int projectIndex){
        vector<string> names = ZNodeStore::get().getProjectNames("/json");
        vector<DataStore::Crumb> crumbs = ZNodeStore::get().loadCrumbs(names.at(projectIndex));
        std::reverse(crumbs.begin(), crumbs.end());
        DataStore::get().setCrumbsForIndex(crumbIndex, crumbs);
    });
}

void ZDataViewController::onFileDrop(int count, const char** paths) {
    string path(paths[0]);
    string ext = getFileExtension(path);


    if (ext == "json") {
        cout << "Data importing..." << endl;
        json j = DataStore::get().parseJsonFromFile(path);
        DataStore::get().storeJson(j, path);
        mListView->addItem(path);
    } else {
        cout << "File type: " << ext << " not supported" << endl;
    }
}

string ZDataViewController::getFileExtension(string path) {
    return path.substr(path.find_last_of(".") + 1) ;
}

