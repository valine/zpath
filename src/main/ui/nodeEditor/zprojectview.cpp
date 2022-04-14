//
// Created by Lukas Valine on 6/18/21.
//

#include <utility>

#include "ui/zprojectview.h"
#include "ui/ztextfield.h"
ZProjectView::ZProjectView(ZView *parent, const function<vector<string>()> &model) : ZView(VIEW_WIDTH, fillParent, parent) {

    mScrollView = new ZScrollView(fillParent, fillParent, this);
    mScrollView->setInnerViewHeight(200);
    mScrollView->setBackgroundColor(transparent);
    mScrollView->setMarginTop(25);

    mModelInterface = model;
    setBackgroundColor(grey1);
    setElevation(1.0);
    setCornerRadius(vec4(5,1,1,1));

    mNewProjectBtn = new ZButton("New", this);
    mNewProjectBtn->setMaxWidth(getMaxWidth() * 0.55);
    mNewProjectBtn->setCornerRadius(vec4(5));
    mNewProjectBtn->setMargin(vec4(2, 2, 2, 2));
    mNewProjectBtn->setBackgroundColor(grey0);
    mNewProjectBtn->setGravity(bottomLeft);
    mNewProjectBtn->setOnClick([this](ZView* sender){
        addUnsavedProject();
    });

    mDeleteProjectBtn = new ZButton("Delete", this);
    mDeleteProjectBtn->setMaxWidth(getMaxWidth() * 0.40);
    mDeleteProjectBtn->setCornerRadius(vec4(5));
    mDeleteProjectBtn->setMargin(vec4(2, 2, 2, 2));
    mDeleteProjectBtn->setGravity(bottomRight);
    mDeleteProjectBtn->setBackgroundColor(grey0);
    mDeleteProjectBtn->setOnClick([this](ZView* sender){
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

    mMinimise = new ZButton(">", this);
    mMinimise->setGravity(topRight);
    mMinimise->setBackgroundColor(grey2);
    mMinimise->setCornerRadius(vec4(5, 0, 0, 0));
    mMinimise->setOnClick([this](ZView* sender){
        toggleMinimise();
    });

    vector<string> names = model();
    for (const auto& name : names) {
        addProject(name);
    }
    addUnsavedProject();
    mScrollView->getInnerView()->refreshMargins();
    mScrollView->setMarginBottom(mNewProjectBtn->getMaxHeight());
    selectProject(mProjectViews.at(mProjectViews.size() - 1));

}

void ZProjectView::minimiseView() {
    mMinimise->setText("<");
    this->setMaxWidth(MIN_WIDTH);
    this->mScrollView->setVisibility(false);
    mNewProjectBtn->setVisibility(false);
    mDeleteProjectBtn->setVisibility(false);
    mMinimise->setGravity(bottomLeft);
    this->setCornerRadius(vec4(0));
    this->setBackgroundColor(transparent);
}

void ZProjectView::maximiseView() {
    mMinimise->setText(">");
    this->setMaxWidth(VIEW_WIDTH);
    this->mScrollView->setVisibility(true);
    mDeleteProjectBtn->setVisibility(true);
    mNewProjectBtn->setVisibility(true);
    mMinimise->setGravity(topRight);
    this->setCornerRadius(vec4(5,1,1,1));
    this->setBackgroundColor(grey1);

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
    if (mSelectedTag == -1) {
        selectProject(mProjectViews.at(mProjectViews.size() - 1));
    }
}

void ZProjectView::addProject(const string& name) {
    auto project = newProjectView();
    project->setText(getFileName(name));
    project->setFocusMode(ZTextField::FocusMode::doubleClick);
    project->setTextMode(ZTextField::TextMode::field);
    project->setMaxWidth(200);
    project->setMaxHeight(24);
    project->setLineTopMargin(TOP_MARGIN);
    project->setElevation(0);
    project->setCornerRadius(vec4(5));
    project->setMargin(vec4(2, 2, 2, 0));
    project->setBackgroundColor(grey1);
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
    unsavedPrj->setLineTopMargin(TOP_MARGIN);
    unsavedPrj->mTitle->setLineTopMargin(TOP_MARGIN);
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

int ZProjectView::getSelectedProject() {
    return mSelectedTag;
}

void ZProjectView::setOnProjectSelected(function<void(int, string)> listener) {
    mOnProjectSelected = std::move(listener);
}

void ZProjectView::toggleMinimise() {
    if (this->getMaxWidth() > MIN_WIDTH) {
        minimiseView();
    } else {
        maximiseView();
    }

    onWindowChange(getWindowWidth(), getWindowHeight());
}
