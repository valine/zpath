//
// Created by lukas on 1/28/22.
//

#include <utils/znodestore.h>

#include <utility>
#include "ui/zdataviewcontroller.h"
#include "utils/datastore.h"
#include "ui/zlistview.h"

#include <filesystem>
#if __APPLE__
using std::__fs::filesystem::directory_iterator;
using std::__fs::filesystem::directory_entry;
using std::__fs::filesystem::path;
using std::__fs::filesystem::relative;
#else
using std::filesystem::directory_iterator;
using std::filesystem::directory_entry;
using std::filesystem::path;
using std::filesystem::relative;

#endif

void ZDataViewController::onCreate() {
    ZViewController::onCreate();
    mListView = new ZListView(this);
    mListView->setMargin(10);

    vector<string> files = ZSettings::get().loadFileList();

    thread t(&ZDataViewController::loadDataFiles, this);
    t.detach();

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

        if (names.size() <= index) {
            return grey1;
        }
        vector<DataStore::Crumb> crumbs = ZNodeStore::get().loadCrumbs(names.at(index));
        std::reverse(crumbs.begin(), crumbs.end());
        bool valid = DataStore::get().crumbsValidForIndex(projectIndex, crumbs);

        if (valid) {
            return gold;
        } else {
            return grey1;
        }
    });


    DataStore::get().addDataChangeListener([this](string path){
        string ext = getFileExtension(path);
        mListView->addItem(path);
        int index = mListView->getListSize();
        vector<int> formats = ZSettings::get().loadFormatList();
        mListView->updateNamesAtIndex(index - 1);

        if (formats.size() > index && formats.at(index) >= 0) {
            mListView->selectItemDropDown(index, formats.at(index));
            vector<string> names = ZNodeStore::get().getProjectNames("/json");
            vector<DataStore::Crumb> crumbs = ZNodeStore::get().loadCrumbs(names.at(formats.at(index)));
            std::reverse(crumbs.begin(), crumbs.end());
            DataStore::get().setCrumbsForIndex(index, crumbs);
        }

        ZSettings::get().saveFileList(mListView->getItems());
    });
}

void ZDataViewController::loadDataFile(string apath) {
    string ext = getFileExtension(apath);

    path p = path(apath);
    if (p.is_relative()) {
        apath = ZSettings::get().getResourcePath() + apath;
    }

    cout << "Data importing..." << endl;
    if (ext == "json") {
        json j = DataStore::get().parseJsonFromFile(apath);
        int index = mListView->getListSize();
        DataStore::get().storeData(j, apath + to_string(index));
    } else if (ext == "csv") {
        json j = DataStore::get().parseCsvFile(apath);
        int index = mListView->getListSize();
        DataStore::get().storeData(j, apath + to_string(index));
    } else if (ext == "jpg" || ext == "png") {
        const char *c = apath.c_str();
        ZUtil::Image img = ZUtil::loadTexture(c);
        DataStore::get().storeData(img, apath);
    } else {
        cout << "File type: " << ext << " not supported" << endl;
    }
}

void ZDataViewController::onFileDrop(int count, const char** paths) {
    string path(paths[0]);
    loadDataFile(path);
}

void ZDataViewController::loadDataFiles() {
    vector<string> files = ZSettings::get().loadFileList();
    for (const string& file : files) {
        ZDataViewController::loadDataFile(file);
    }
}

string ZDataViewController::getFileExtension(string path) {
    return path.substr(path.find_last_of(".") + 1) ;
}

