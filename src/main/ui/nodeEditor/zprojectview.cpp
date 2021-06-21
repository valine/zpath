//
// Created by Lukas Valine on 6/18/21.
//

#include "ui/zprojectview.h"
#include "ui/ztextfield.h"
ZProjectView::ZProjectView(ZView *parent, const function<vector<string>()>& model)  : ZScrollView(120, fillParent, parent) {
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

    addUnsavedProject();

    vector<string> names = model();
    for (const auto& name : names) {
        addProject(name);
    }

    getInnerView()->refreshMargins();
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
    mAllProjects.push_back(project);
    project->setIndexTag(mProjectIdInc);
    mProjectMap.insert({mProjectIdInc, name});
    mProjectIdInc++;
}

void ZProjectView::addUnsavedProject() {
    auto unsavedPrj = new ZTextField(this);
    unsavedPrj->setText("Unsaved Project");
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
    mAllProjects.push_back(unsavedPrj);
    unsavedPrj->setOnReturn([](string name){
        cout << name << endl;
    });
    unsavedPrj->setIndexTag(mProjectIdInc);
    mProjectMap.insert({mProjectIdInc, ""});
    mProjectIdInc++;
}

void ZProjectView::selectProject(ZView *sender) {
    for (auto project : mAllProjects) {
        project->setBackgroundColor(white);
    }
    sender->setBackgroundColor(highlight);
    if (mOnProjectSelected != nullptr) {
        mOnProjectSelected(sender->getIndexTag(),
                           mProjectMap.at(sender->getIndexTag()));
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
    mOnProjectSelected = listener;
}
