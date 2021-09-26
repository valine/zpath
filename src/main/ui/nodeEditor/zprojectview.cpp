//
// Created by Lukas Valine on 6/18/21.
//

#include <utility>

#include "ui/zprojectview.h"
#include "ui/ztextfield.h"
ZProjectView::ZProjectView(ZView *parent, const function<vector<string>()> &model) : ZScrollView(120, fillParent, parent) {
    mModelInterface = model;
    setBackgroundColor(white);
    setElevation(1.0);
    setCornerRadius(vec4(5,1,1,1));
    setInnerViewHeight(200);

    auto newProject = new ZTextField(this);
    newProject->setText("New Project");
    newProject->setFocusMode(ZTextField::FocusMode::doubleClick);
    newProject->setMaxWidth(200);
    newProject->setCornerRadius(vec4(5));
    newProject->setMargin(vec4(2, 2, 2, 0));
    newProject->setBackgroundColor(grey);
    newProject->setOnClick([this](ZView* sender){
        addUnsavedProject();
    });

    vector<string> names = model();
    for (const auto& name : names) {
        addProject(name);
    }

    addUnsavedProject();
    getInnerView()->refreshMargins();
}

void ZProjectView::reloadProjects() {
//
//    for (auto view : mProjectViews) {
//        view->setIndexTag(-1);
//        view->setVisibility(false);
//    }
//
//    vector<string> names = mModelInterface();
//    for (const auto& name : names) {
//        addProject(name);
//    }
}

void ZProjectView::onLayoutFinished() {
    ZView::onLayoutFinished();
    selectProject(mProjectViews.at(mProjectViews.size() - 1));
}

void ZProjectView::addProject(string name) {
    auto project = new ZTextField(this);
    project->setText(getFileName(name));
    project->setFocusMode(ZTextField::FocusMode::doubleClick);
    project->setTextMode(ZTextField::TextMode::field);
    project->setMaxWidth(200);
    project->setCornerRadius(vec4(5));
    project->setMargin(vec4(2, 2, 2, 0));
    project->setBackgroundColor(white);
    project->setOnClick([this](ZView* sender){
        selectProject(sender);
    });

    project->setOnReturn([this, project](string name) {
        if (mOnProjectRenamed != nullptr) {
            string path = mOnProjectRenamed(std::move(name), project->getIndexTag());
            mNameMap.at(project->getIndexTag()) = path;
            cout << path << endl;
        }
    });

    mProjectViews.push_back(project);
    project->setIndexTag(mProjectIdInc);
    mNameMap.insert({mProjectIdInc, name});
    mIDMap.insert({name, mProjectIdInc});
    mProjectIdInc++;
}

void ZProjectView::addUnsavedProject() {
    auto unsavedPrj = new ZTextField(this);
    unsavedPrj->setTitleText("Unsaved Project");
    unsavedPrj->setFocusMode(ZTextField::FocusMode::doubleClick);
    unsavedPrj->setOutlineType(WireType::outline);
    unsavedPrj->setTextMode(ZTextField::TextMode::field);
    unsavedPrj->setMaxWidth(200);
    unsavedPrj->setCornerRadius(vec4(5));
    unsavedPrj->setMargin(vec4(2, 2, 2, 0));
    unsavedPrj->setBackgroundColor(white);
    unsavedPrj->setOutlineColor(grey);
    unsavedPrj->setLineWidth(0.5);
    unsavedPrj->setOnClick([this](ZView* sender){
        selectProject(sender);
    });
    mProjectViews.push_back(unsavedPrj);
    unsavedPrj->setOnReturn([this, unsavedPrj](string name) {
        if (mOnProjectSaved != nullptr) {
           string path = mOnProjectSaved(name, unsavedPrj->getIndexTag());
           mNameMap.at(unsavedPrj->getIndexTag()) = path;
           unsavedPrj->setOutlineType(none);
           unsavedPrj->invalidate();
           unsavedPrj->setLineWidth(0.0);
        }
        cout << name << endl;
    });
    unsavedPrj->setIndexTag(mProjectIdInc);
    mNameMap.insert({mProjectIdInc, ""});
    mProjectIdInc++;
}

void ZProjectView::selectProject(ZView *sender) {
    for (auto project : mProjectViews) {
        project->setBackgroundColor(white);
    }
    sender->setBackgroundColor(highlight);
    if (mOnProjectSelected != nullptr) {
         mOnProjectSelected(sender->getIndexTag(),
                           mNameMap.at(sender->getIndexTag()));
    }
}

string ZProjectView::getFileName(const string& s) {
    char sep = '/';

#ifdef _WIN32
    sep = '\\';
#endif

    size_t i = s.rfind(sep, s.length());
    if (i != string::npos) {
        string name = (s.substr(i+1, s.length() - i));

        if (name.substr(name.length() - 5, name.length()) == "zpath") {
            return name.substr(0, name.length() - 6);
        }
    }

    return("");
}

void ZProjectView::setOnProjectSelected(function<void(int, string)> listener) {
    mOnProjectSelected = std::move(listener);
}
