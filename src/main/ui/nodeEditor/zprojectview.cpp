//
// Created by Lukas Valine on 6/18/21.
//

#include <utility>

#include "ui/zprojectview.h"
#include "ui/ztextfield.h"
ZProjectView::ZProjectView(ZView *parent, const function<vector<string>()> &model) : ZView(125, fillParent, parent) {

    mScrollView = new ZScrollView(fillParent, fillParent, this);
    mScrollView->setInnerViewHeight(200);
    mScrollView->setBackgroundColor(transparent);

    mModelInterface = model;
    setBackgroundColor(grey1);
    setElevation(1.0);
    setCornerRadius(vec4(5,1,1,1));

    auto newProjectBtn = new ZButton("New", this);
    newProjectBtn->setMaxWidth(getMaxWidth() * 0.55);
    newProjectBtn->setCornerRadius(vec4(5));
    newProjectBtn->setMargin(vec4(2, 2, 2, 2));
    newProjectBtn->setBackgroundColor(grey0);
    newProjectBtn->setGravity(bottomLeft);
    newProjectBtn->setOnClick([this](ZView* sender){
        addUnsavedProject();
    });

    auto deleteProjectBtn = new ZButton("Delete", this);
    deleteProjectBtn->setMaxWidth(getMaxWidth() * 0.40);
    deleteProjectBtn->setCornerRadius(vec4(5));
    deleteProjectBtn->setMargin(vec4(2, 2, 2, 2));
    deleteProjectBtn->setGravity(bottomRight);
    deleteProjectBtn->setBackgroundColor(grey0);
    deleteProjectBtn->setOnClick([this](ZView* sender){
        if (mOnProjectDelete(mNameMap.at(mSelectedTag), mSelectedTag)) {
            auto toDelete = mProjectViews.at(mSelectedTag);
            mScrollView->getInnerView()->removeSubView(toDelete);
            mRecycleBin.push(toDelete);
            mScrollView->getInnerView()->refreshMargins();
            toDelete->setIndexTag(-1);

            ZView* lastProject = getLastProject();
            if (lastProject == nullptr) {
                addUnsavedProject();
                selectProject(getLastProject());
            } else {
                selectProject(lastProject);
            }
        }
    });

    vector<string> names = model();
    for (const auto& name : names) {
        addProject(name);
    }
    addUnsavedProject();
    mScrollView->getInnerView()->refreshMargins();
    mScrollView->setMarginBottom(newProjectBtn->getMaxHeight());
}

ZView* ZProjectView::getLastProject() {
    if (mProjectViews.empty()) {
        return nullptr;
    }
    int index = mProjectViews.size() - 1;
    while (index >= 0 && mProjectViews.at(index)->getIndexTag() == -1 ) {
        index--;
    }

    if (index < 0) {
        return nullptr;
    }
    return mProjectViews.at(index);
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

void ZProjectView::addProject(const string& name) {
    auto project = newProjectView();
    project->setText(getFileName(name));
    project->setFocusMode(ZTextField::FocusMode::doubleClick);
    project->setTextMode(ZTextField::TextMode::field);
    project->setMaxWidth(200);
    project->setMaxHeight(24);
    project->setLineTopMargin(2.5);
    project->setElevation(0);
    project->setCornerRadius(vec4(5));
    project->setMargin(vec4(2, 2, 2, 0));
    project->setBackgroundColor(transparent);
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
    auto unsavedPrj = newProjectView();
    unsavedPrj->setText("");
    unsavedPrj->setTitleText("Unsaved Project");
    unsavedPrj->setFocusMode(ZTextField::FocusMode::doubleClick);
    unsavedPrj->setTextMode(ZTextField::TextMode::field);
    unsavedPrj->setMaxWidth(200);
    unsavedPrj->setMaxHeight(24);
    unsavedPrj->setLineTopMargin(2.5);
    unsavedPrj->mTitle->setLineTopMargin(2.5);
    unsavedPrj->setCornerRadius(vec4(5));
    unsavedPrj->setMargin(vec4(2, 2, 0, 0));
    unsavedPrj->setBackgroundColor(gold);
    unsavedPrj->setOnClick([this](ZView* sender){
        selectProject(sender);
    });
    mProjectViews.push_back(unsavedPrj);
    unsavedPrj->setOnReturn([this, unsavedPrj](string name) {
        if (mOnProjectSaved != nullptr) {
           string path = mOnProjectSaved(std::move(name), unsavedPrj->getIndexTag());
           mNameMap.at(unsavedPrj->getIndexTag()) = path;
           unsavedPrj->setBackgroundColor(grey1);
           unsavedPrj->invalidate();
           selectProject(unsavedPrj);
        }
    });
    unsavedPrj->setIndexTag(mProjectIdInc);
    mNameMap.insert({mProjectIdInc, ""});
    mProjectIdInc++;
    mScrollView->getInnerView()->refreshMargins();
}

ZTextField* ZProjectView::newProjectView() {
    if (mRecycleBin.empty()) {
        return new ZTextField(mScrollView);
    } else {
        auto view = mRecycleBin.front();
        mScrollView->addSubView(view);
        mRecycleBin.pop();
        return view;
    }
}

void ZProjectView::selectProject(ZView *sender) {
    for (auto project : mProjectViews) {

        if (project->getIndexTag() != -1) {
            // Unsaved project
            if (mNameMap.at(project->getIndexTag()).empty()) {
                project->setBackgroundColor(gold);
            } else {
                project->setBackgroundColor(grey1);
            }
        }
    }
    sender->setBackgroundColor(highlight);
    if (mOnProjectSelected != nullptr) {
         mOnProjectSelected(sender->getIndexTag(),
                           mNameMap.at(sender->getIndexTag()));
    }

    mSelectedTag = sender->getIndexTag();
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
