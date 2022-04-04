//
// Created by Lukas Valine on 6/18/21.
//

#ifndef ZPATH_ZPROJECTVIEW_H
#define ZPATH_ZPROJECTVIEW_H


static const double TOP_MARGIN = 2.0;

#include "zview.h"
#include "zscrollview.h"
#include "ui/ztextfield.h"
#include <queue>
#include <utility>

class ZProjectView : public ZView {

public :
    ZProjectView(ZView *parent, const function<vector<string>()> &model);
    string getFileName(const string &s);
    void setOnProjectSelected(function<void(int index, string path)> listener);

    void setOnProjectSaved(function<string(string name, int index)> listener) {
        mOnProjectSaved = std::move(listener);
    }

    void setOnProjectRenamed(function<string(string name, int index)> rename) {
        mOnProjectRenamed = std::move(rename);
    }

    void setOnProjectDelete(function<bool(string name, int index)> rename) {
        mOnProjectDelete = std::move(rename);
    }
    int getSelectedProject();

    void onLayoutFinished() override;
private:
    ZScrollView* mScrollView;
    vector<ZTextField*> mProjectViews;
    queue<ZTextField*> mRecycleBin;
    function<vector<string>()> mModelInterface;
    function<void(int index, string path)> mOnProjectSelected = nullptr;
    function<string(string name, int index)> mOnProjectRenamed = nullptr;
    function<bool(string name, int index)> mOnProjectDelete = nullptr;
    function<string(string name, int index)> mOnProjectSaved = nullptr;
    int mProjectIdInc = 0;
    map<int, string> mNameMap;
    map<string, int> mIDMap;
    int mSelectedTag = 0;

    void selectProject(ZView *sender);

    void addProject(const string& name);

    void addUnsavedProject();

    void reloadProjects();

    ZTextField *newProjectView();

    ZView *getLastProject();

};


#endif //ZPATH_ZPROJECTVIEW_H
