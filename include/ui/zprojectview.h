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
    ZProjectView(ZView *parent, const function<vector<string>()>& model);
    string getFileName(const string &s);
    void setOnProjectSelected(function<void(int index, string path)> listener);

private:
    vector<ZTextField*> mAllProjects;
    function<vector<string>()> mModelInterface;
    function<void(int index, string path)> mOnProjectSelected;
    int mProjectIdInc = 0;
    map<int, string> mProjectMap;

    void selectProject(ZView *sender);

    void addProject(string name);

    void addUnsavedProject();

    void reloadProjects();
};


#endif //ZPATH_ZPROJECTVIEW_H
