//
// Created by Lukas Valine on 6/18/21.
//

#ifndef ZPATH_ZPROJECTVIEW_H
#define ZPATH_ZPROJECTVIEW_H


#include "zview.h"
#include "zscrollview.h"
#include "ui/ztextfield.h"
#include <queue>

class ZProjectView : public ZScrollView {

public :
    ZProjectView(ZView *parent, const function<vector<string>()> &model);
    string getFileName(const string &s);
    void setOnProjectSelected(function<void(int index, string path)> listener);

    void setOnProjectSaved(function<string(string name, int index)> listener) {
        mOnProjectSaved = listener;
    }

    void setOnProjectRenamed(function<string(string name, int index)> rename) {
        mOnProjectRenamed = rename;
    }

    void onLayoutFinished() override;
private:
    vector<ZTextField*> mProjectViews;
    function<vector<string>()> mModelInterface;
    function<void(int index, string path)> mOnProjectSelected = nullptr;
    function<string(string name, int index)> mOnProjectRenamed = nullptr;
    function<string(string name, int index)> mOnProjectSaved = nullptr;
    int mProjectIdInc = 0;
    map<int, string> mNameMap;
    map<string, int> mIDMap;

    void selectProject(ZView *sender);

    void addProject(string name);

    void addUnsavedProject();

    void reloadProjects();

};


#endif //ZPATH_ZPROJECTVIEW_H
