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

    vector<string> files = ZSettings::get().loadFileList();
    for (const string& file : files) {
        thread t(&ZDataViewController::loadDataFile, this, file);
        t.detach();
    }

    mListView->setCrumbInterface([](){
        vector<string> names = ZNodeStore::get().getProjectNames("/json");
        return names;
    });

    mListView->setOnCrumbChange([this](int crumbIndex, int projectIndex){
        vector<string> names = ZNodeStore::get().getProjectNames("/json");
        vector<DataStore::Crumb> crumbs = ZNodeStore::get().loadCrumbs(names.at(projectIndex));
        std::reverse(crumbs.begin(), crumbs.end());
        DataStore::get().setCrumbsForIndex(crumbIndex, crumbs);

        ZSettings::get().saveFormatList(mListView->getFormats());
    });

    mListView->setColorInterface([this](int index, int projectIndex){
        vector<string> names = ZNodeStore::get().getProjectNames("/json");
        vector<DataStore::Crumb> crumbs = ZNodeStore::get().loadCrumbs(names.at(index));
        std::reverse(crumbs.begin(), crumbs.end());
        bool valid = DataStore::get().crumbsValidForIndex(projectIndex, crumbs);

        if (valid) {
            return gold;
        } else {
            return grey1;
        }
    });
}

void ZDataViewController::onFileDrop(int count, const char** paths) {
    string path(paths[0]);
    loadDataFile(path);
    ZSettings::get().saveFileList(mListView->getItems());
}

void ZDataViewController::loadDataFile(string path) {
    string ext = getFileExtension(path);


    if (ext == "json") {
        cout << "Data importing..." << endl;
        json j = DataStore::get().parseJsonFromFile(path);
        DataStore::get().storeJson(j, path);
        mListView->addItem(path);

        vector<int> formats = ZSettings::get().loadFormatList();

        int index = mListView->getItems().size() - 1;
        mListView->updateNamesAtIndex(index);

        if (formats.size() > index && formats.at(index) >= 0) {
            mListView->selectItemDropDown(index, formats.at(index));

            vector<string> names = ZNodeStore::get().getProjectNames("/json");
            vector<DataStore::Crumb> crumbs = ZNodeStore::get().loadCrumbs(names.at(formats.at(index)));
            std::reverse(crumbs.begin(), crumbs.end());
            DataStore::get().setCrumbsForIndex(index, crumbs);
        }
    } else {
        cout << "File type: " << ext << " not supported" << endl;
    }
}

string ZDataViewController::getFileExtension(string path) {
    return path.substr(path.find_last_of(".") + 1) ;
}

