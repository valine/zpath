#include <utility>

//
// Created by lukas on 8/4/20.
//
//#include <nlohmann/json.hpp>
//using json = nlohmann::json;


#include <ui/zlineview.h>
#include <ui/zlabel.h>
#include <ui/zbutton.h>
#include <ui/znodeview.h>
#include <ui/zdropdown.h>
#include <ui/zcheckbox.h>
#include <ui/zmagnitudepicker.h>
#include <thread>
#include <utils/zsettingsstore.h>
#include <ui/ztextfield.h>
#include <ui/zdrawer.h>
#include <utils/zcornerrenderer.h>
#include "ui/znodeeditor.h"
#include <utils/znodeutil.h>
#include <utils/znodestore.h>
#include <ui/zprojectview.h>
#include <utils/zgridrenderer.h>
#include "utils/casutil.h"
#include "utils/zutil.h"
#include "ui/nodetype.h"

ZNodeEditor::ZNodeEditor(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight, parent) {

    setBackgroundColor(ZSettings::get().getBackgroundColor());


    // Checkered background
    mCheckerView = new ZView(fillParent, fillParent, this);
    mCheckerView->setBackgroundColor(bg);
    ZGridRenderer renderer = ZGridRenderer::get();
    auto tex = renderer.create();
    mCheckerView->setBackgroundImage(tex);
    mCheckerView->setYOffset(NODE_CONTAINER_OFFSET);
    mCheckerView->setXOffset(100);
    mCheckerView->getBackgroundImage()->setScale(GRID_SCALE);

    mNodeContainer = new ZScrollView(fillParent, fillParent, this);
    mLineContainer = new ZView(fillParent, fillParent, this);
    mNodeContainer->setYOffset(NODE_CONTAINER_OFFSET);
    mNodeContainer->setXOffset(100);

    mNodeContainer->setUseDynamicSize(false);
    mNodeContainer->setInnerViewHeight(5000);
    mNodeContainer->setBackgroundColor(transparent);

    mTmpLine = new ZLineView(vec2(20, 20), vec2(200, 200), mLineContainer);
    mTmpLine->setVisibility(false);

    mBoxSelect = new ZView(100, 100, mLineContainer);
    mBoxSelect->setOutlineType(WireType::outline);
    mBoxSelect->setOutlineColor(ZSettings::get().getHighlightColor());
    mBoxSelect->setVisibility(false);
    mBoxSelect->setAllowNegativeSize(true);

    mCursorView = new ZCursorView(mLineContainer);
    mCursorView->setVisibility(false);
    mCursorView->setClippingEnabled(false);

    auto* line = new ZLineView(vec2(0), vec2(0), mLineContainer);
    mLineBucket.push_back(line);

    mProjectBrowser = new ZProjectView(this, []() {
        return ZNodeStore::get().getProjectNames();
    });

    mProjectBrowser->setGravity(bottomRight);
    mProjectBrowser->setYOffset(0);
    mProjectBrowser->setMaxHeight(400);
    mProjectBrowser->setMarginTop(200);
    mProjectBrowser->onWindowChange(getWindowWidth(), getWindowWidth());
    mProjectBrowser->setOnProjectSelected([this](int index, string path) {
        selectProject(index, path);
    });

    mProjectBrowser->setOnProjectSaved([this](string name, int index){
        set<ZNodeView*> projectNodes;
        for (auto node : mNodeViews) {
            if (node->getProjectID() == index) {
                projectNodes.insert(node);
            }
        }
        return ZNodeStore::get().saveGraph(projectNodes, name, false);
    });

    mProjectBrowser->setOnProjectRenamed([this](string name, int index){
        mProjectPath =  ZNodeStore::get().renameProject(mProjectPath, name);
        return mProjectPath;
    });

    mProjectBrowser->setOnProjectDelete([this](string path, int index){
        bool success = true;
        if (!path.empty()) {
            success = ZNodeStore::get().deleteProject(path);
        }

        if (success) {
            set<ZNodeView *> toDelete;
            for (auto node : mNodeViews) {
                if (node->getProjectID() == index) {
                    toDelete.insert(node);
                }
            }
            for (auto node : toDelete) {
                deleteNode(node);
            }
        } else {
            cout << "bad delete " << path << endl;
        }

        return success;
    });
    mSelectedProject = mProjectBrowser->getSelectedProject();

    auto* headerBackground = new ZView(fillParent, 25, this);
    headerBackground->setBackgroundColor(grey1);

    mHeader = new ZView(fillParent, fillParent, this);

    auto* viewDropDown = new ZDropDown(100,25, {"Snap to Nodes"}, mHeader);
    viewDropDown->setOffset(0, 0);
    viewDropDown->setTitle("View");
    //viewDropDown->wrapTitle();
    viewDropDown->setDynamicTitle(false);
    viewDropDown->setOnItemChange([this](int index){
        // Center mCheckerView
        if (index == 0) {
            snapViewToNodes();
        }
    });

    thread evalThread = thread(ZNodeEditor::startEvaluation, this);
    evalThread.detach();

    mButtonPanel = new ZLinearLayout(80, 200, this);
    mButtonPanel->setGravity(topRight);
    mButtonPanel->setYOffset(30);
    mButtonPanel->setXOffset(5);

    auto laplaceBtn = new ZButton("Laplace", mButtonPanel);
    float cr = (float) laplaceBtn->getMaxHeight() / 2;
    laplaceBtn->setMaxWidth(fillParent);
    laplaceBtn->setCornerRadius(vec4(cr, cr, 0, 0));
    laplaceBtn->setMaxHeight(22);
    laplaceBtn->setOnClick([this](){
        // Only works if one node selected
        if (mSelectedNodes.size() == 1) {
            ZNodeView *root = (*mSelectedNodes.begin());
            string exp = ZNodeUtil::get().graphToExpString(root, true);
            string laplace = "laplace(" + exp + ")";
            string result = ZUtil::replace(CasUtil::get().evaluate(laplace), "\n", "");
            string zResult = ZUtil::replace(result, "x", "z");
            string heatResult = "heat(" + zResult + ")";
            addNodeGraph(ZNodeUtil::get().expStringToGraph(heatResult).at(0), vec2(10), 0);
        }
    });

    auto derivativeBtn = new ZButton("Derivative", mButtonPanel);
    derivativeBtn->setMaxWidth(fillParent);
    derivativeBtn->setCornerRadius(vec4(1));
    derivativeBtn->setMaxHeight(22);
    derivativeBtn->setMarginTop(1);
    derivativeBtn->setOnClick([this](){
        // Only works if one node selected
        if (mSelectedNodes.size() == 1) {
            ZNodeView *root = (*mSelectedNodes.begin());
            string exp = ZNodeUtil::get().graphToExpString(root, true);
            string diff = "diff(" + exp + ")";
            string result = ZUtil::replace(CasUtil::get().evaluate(diff), "\n", "");
            addNodeGraph(ZNodeUtil::get().expStringToGraph(result).at(0),
                         vec2(10), 0);
        }
    });

    auto simplifyBtn = new ZButton("Simplify", mButtonPanel);
    simplifyBtn->setMaxWidth(fillParent);
    simplifyBtn->setCornerRadius(vec4(1));
    simplifyBtn->setMaxHeight(22);
    simplifyBtn->setMarginTop(1);
    simplifyBtn->setOnClick([this](){
        // Only works if one node selected
        if (mSelectedNodes.size() == 1) {
            ZNodeView *root = (*mSelectedNodes.begin());
            string exp = ZNodeUtil::get().graphToExpString(root, true);
            string diff = "simplify(" + exp + ")";
            string result = ZUtil::replace(CasUtil::get().evaluate(diff), "\n", "");
            addNodeGraph(ZNodeUtil::get().expStringToGraph(result).at(0), vec2(10), 0);
        }
    });

    // Button example
    auto* addNodeBtn = new ZButton("Add node", mButtonPanel);
    addNodeBtn->setMarginTop(1);
    addNodeBtn->setMaxHeight(22);
    addNodeBtn->setCornerRadius(vec4(0,0,cr,cr));
    addNodeBtn->setOnClick([this](){
        addNode(mLastType);
    });

    mButtonPanel->refreshMargins();

    // Magnitude picker work
    mMagnitudePicker = new ZMagnitudePicker(this);
    mMagnitudePicker->setVisibility(false);

    mExpressionField = new ZTextField(this);
    mExpressionField->setGravity(Gravity::bottomLeft);
    mExpressionField->setMaxWidth(360);
    mExpressionField->setXOffset(DRAWER_WIDTH + 5);
    mExpressionField->setOutlineType(WireType::outline);
    mExpressionField->setMaxHeight(20);
    mExpressionField->setMargin(vec4(2));
    mExpressionField->setElevation(1.0);
    mExpressionField->setMargin(vec4(15,15,140,15));
    mExpressionField->setTitleText("Enter expression...");
    mExpressionField->setTextMode(ZTextField::TextMode::field);
    mExpressionField->setOnReturn([this](string value) {
       // vector<ZNodeView*> evalNodes = ZNodeUtil::get().expStringToGraph(value);
       // vec2 pos = vec2(700, 300);
       // addNodeGraph(evalNodes.at(0), pos, 0);
    });

    mExpressionField->setOnTextChange([this](string value) {
        int margin = 20;
        vec2 pos = vec2(margin, margin);
        if (mSelectedNodes.size() == 1) {
            ZNodeView* aNode =(*mSelectedNodes.begin());
            pos = aNode->getOffset();
        }

        for (ZNodeView* node : mSelectedNodes) {
            deleteNodeRecursive(node);
        }
        reindexNodes();

        mSelectedNodes.clear();

        vector<ZNodeView*> evalNodes = ZNodeUtil::get().expStringToGraph(value);

        if (!evalNodes.empty()) {
            addNodeGraph(evalNodes.at(0), pos, 0);
        }
    });
}

void ZNodeEditor::setButtonPanelVisibility(bool visible) {
    mButtonPanel->setVisibility(visible);
}

void ZNodeEditor::setNodeTypes(vector<NodeType*> nodeTypes) {
    vector<string> allTypes;
    vector<ZColor> allColors;

    vector<NodeType*> nodesInDrawer;

    int index = 0;
    for (auto type : nodeTypes) {
        if (type->mShowInDrawer) {
            nodesInDrawer.push_back(type);
            allTypes.push_back(type->mName);
            allColors.push_back(type->mColor);
            index++;
        }
    }

    mDrawer = new ZDrawer(this, allTypes, allColors);
    mDrawer->setMarginTop(25);
    mDrawer->setOnItemSelected([this, allTypes, nodesInDrawer](int index){
        vec2 scrollOffset = mNodeContainer->getInnerView()->getTranslation();

        vec2 mousePosition = (getRelativeMouse() / mNodeContainer->getInnerView()->getScale()) - mNodeContainer->getInnerView()->getInnerTranslation();
        vec2 startPosition = (mousePosition) + vec2(mNodeContainer->getMarginTop() - mDrawer->getMaxWidth(), 0) - scrollOffset;
        // startPosition.x = std::max((int) mDrawer->getWidth(), (int) startPosition.x);
        startPosition.y -= 40;
        startPosition.x -= 35;
        ZNodeView* node = addNode(nodesInDrawer.at(index));
        node->setOffset(startPosition);

        selectNode(node);
        mDragNode = node->getIndexTag();
        node->setInitialPosition(startPosition);
        enterGrabMode();

    });

    mDrawer->setOnItemClicked([this, allTypes, nodesInDrawer](int index){
        addNode(nodesInDrawer.at(index));
    });
}

void ZNodeEditor::selectProject(int index, string &path) {
    bool currentProjectSave = !mProjectPath.empty();
    mSelectedProject = index;
    mProjectPath = path;
    mGroupMode = NO_GROUP;

    // A saved project was selected
    if (!path.empty()) {
        set<ZNodeView*> toDelete;
        for (auto node : mNodeViews) {
            if (node->getProjectID() == index) {
                toDelete.insert(node);
            } else if (node->getProjectID() != -1){
                node->setVisibility(false);
            }
        }

        for (auto node : toDelete) {
            deleteNode(node);
        }

        set<ZNodeView *> nodes = ZNodeStore::get().loadGraph(path);
        for (auto node : nodes) {
            node->setProjectID(mSelectedProject);
            addNodeToView(node, false);
        }
    } else {
        for (auto node : mNodeViews) {
            if (node->getProjectID() == index && !node->mIsPartOfGroup) {
                node->setVisibility(true);
                node->invalidateSingleNode();
            } else {
                node->setVisibility(false);
            }
        }
    }

    updateLines();
    onWindowChange(getWindowWidth(), getWindowHeight());
}

void ZNodeEditor::onKeyPress(int key, int scancode, int action, int mods) {
    ZView::onKeyPress(key, scancode, action, mods);
    if (isViewInFocus() && !controlKeyPressed()) {
        return;
    }

    if (key == GLFW_KEY_R && shiftKeyPressed() && action == GLFW_PRESS) {
        addNode(mLastType);
    }

    else if (key == GLFW_KEY_E && shiftKeyPressed()) {
        addNode(mLastType);
    }

    else if (key == GLFW_KEY_N && action == GLFW_RELEASE) {
        mProjectBrowser->toggleMinimise();
        invalidate();
    }

    else if (key == GLFW_KEY_B) {
        enterBoxSelectMode();
    }

    else if (key == GLFW_KEY_ESCAPE) {
        exitBoxSelectMode();
    }

    else if (key == GLFW_KEY_X && !shiftKeyPressed() && action == GLFW_RELEASE) {
        deleteSelectedNodes();
    }

    else if (key == GLFW_KEY_X && shiftKeyPressed() && action == GLFW_RELEASE) {
        deleteSelectedConnections();
    }

    else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        if (mSelectedNodes.empty()) {
            selectAllNodes();
        } else {
            deselectAllNodes();
        }
        invalidate();
    }

    else if (key == GLFW_KEY_SLASH && action == GLFW_RELEASE) {
        mExpressionField->setInFocus();
    }

    else if (key == GLFW_KEY_D && shiftKeyPressed() && action == GLFW_PRESS) {
        duplicateSelectedNodes();
    }

    else if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        selectNodeGraphUnderMouse();
    }

    else if ((key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT)) {
        mInitialOffset = getMouse();
        resetNodeInitialPosition();
    }

    else if (key == GLFW_KEY_TAB && action == GLFW_RELEASE) {
        toggleGroupSelection();
    }

    else if (key == GLFW_KEY_S && controlKeyPressed() && action == GLFW_RELEASE) {
        requestSave();
    }
}

void ZNodeEditor::requestSave() {
    mSavePending = true;
    lock_guard<mutex> guard(mEvalMutex);;
    mEvalConVar.notify_one();
}

void ZNodeEditor::saveProject() {
    set<ZNodeView*> projectNodes;
    for (auto node : mNodeViews) {
        if (node->getProjectID() == mSelectedProject) {
            projectNodes.insert(node);
        }
    }
    ZNodeStore::get().saveGraph(projectNodes, mProjectPath, true);
    mSavePending = false;
}

/**
 * Centers existing nodes in view
 */
void ZNodeEditor::snapViewToNodes() {
    if (!mNodeViews.empty()) {
        vec2 minPos = mNodeViews.at(0)->getOffset();
        vec2 maxPos = mNodeViews.at(0)->getOffset() + mNodeViews.at(0)->getSize();

        for (ZNodeView *node : mNodeViews) {
            minPos = min(node->getOffset(), minPos);
            maxPos = max(node->getOffset() + (node->getSize()), maxPos);
        }

        vec2 center = (maxPos - minPos) / vec2(2);
        vec2 viewCenter = (vec2(mNodeContainer->getWidth(),
                                mNodeContainer->getHeight()) * mNodeContainer->getInnerView()->getScale()) / vec2(2);
        vec2 offset = viewCenter - center;

        for (ZNodeView *node : mNodeViews) {
            node->setOffset((node->getOffset() - minPos) + offset);
        }

        getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());

    }
}

void ZNodeEditor::onSizeChange() {
    ZView::onSizeChange();
    updateLines();
}

/**
 * Add a graph to the node view. This is usually a list of pre-connected
 * nodes
 * @param nodes
 */
void ZNodeEditor::addNodeGraph(ZNodeView *root, vec2 position, int depth) {

    if (depth == 0) {
        mTmpNodeOffsetYR.clear();
        mTmpNodes.clear();
        mTmpNodeOffsetX = 0;
        mTmpNodeOffsetY = 0;
    }
    // First node from eval is always root
    addNodeToView(root, false);
    root->setOffset(position);
    root->setInitialPosition(position);

    if (depth == 0) {
        deselectAllNodes();
        selectNode(root);
    } else {
        deselectNode(root);
    }

    vector<ZNodeView*> children = root->getChildren();

    int margin = 20;
    if (mTmpNodeOffsetYR.size() <= depth && !children.empty()) {
        mTmpNodeOffsetYR.push_back(position.y);
        mTmpNodes.push_back(vector<ZNodeView*>());
    }

    if (root->getOffsetX() < 0) {
        mTmpNodeOffsetX = std::min(mTmpNodeOffsetX, (int) position.x);
    }

    for (ZNodeView* node : children) {
        if (node != nullptr) {
            vec2 nodeSize = node->getType()->mNodeSize;
            addNodeGraph(node, vec2(position.x,
                                    mTmpNodeOffsetYR.at(depth)) -
                               (vec2(nodeSize.x, 0) + vec2(margin, 0)),
                         depth + 1);
            mTmpNodeOffsetYR.at(depth) += nodeSize.y + margin;
            mTmpNodes.at(depth).push_back(node);

            if (node->getOffsetX() - margin < 0) {
                mTmpNodeOffsetX = std::min(mTmpNodeOffsetX, (int) node->getOffsetX() - margin);
            }
        }
    }

    if (!children.empty()) {
        auto layer = mTmpNodes.at(depth);
        int span = layer.at(layer.size() - 1)->getLocalBottom() - layer.at(0)->getLocalTop();
        mTmpNodeOffsetY = std::max(mTmpNodeOffsetY, (int) span);
    }

    if (depth == 0) {
        int yOffset = 0;
        int rootCenter = ((root->getMaxHeight()) / 2);
        int top = (mTmpNodeOffsetY / 2);
        if (((position.y - margin) - top) < 0) {
            yOffset = ((position.y - margin) + rootCenter) - top;
        }

        root->setXOffset(root->getOffsetX() - mTmpNodeOffsetX);
        for (vector<ZNodeView*> layer : mTmpNodes) {
            if (!layer.empty()) {

                for (ZNodeView *node : layer) {
                    node->setXOffset(node->getOffsetX() - mTmpNodeOffsetX);
                    node->setInitialPosition(node->getOffset());
                }
            }
        }

        cleanupGraph(root);

       // root->invalidateSingleNode();
        getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
        updateLines();

    }
}

void ZNodeEditor::cleanupGraph(ZNodeView *root) {
    root->setYOffset(0);
    centerGraph(root, 0);
    adjustGraphLayout(root, 0);
    fixGraphOverlap();

    float graphTop = root->getOffsetY();
    float graphBottom = root->getLocalBottom();

    for (auto layer : mTmpNodes) {
        for (auto node : layer) {
            graphTop = std::min((float) node->getOffsetY(), graphTop);
            graphBottom = std::max((float) node->getLocalBottom(), graphBottom);
        }
    }

    float margin = 40;
    root->offsetBy(0,  -graphTop + margin);
    for (auto layer : mTmpNodes) {
        for (auto node : layer) {
            node->offsetBy(0,  -graphTop + margin);
            mNodeContainer->setInnerViewHeight(
                    std::max(graphBottom - graphTop + (margin * 2),
                             (float) mNodeContainer->getInnerView()->getMaxHeight()));
        }
    }
}

void ZNodeEditor::fixGraphOverlap() {
    set<int> spaceTaken;
    for (auto layer : mTmpNodes) {
        std::sort(layer.begin(), layer.end(), compareNodes);
        for (int i = 0; i < layer.size() - 1; i++) {
            auto node = layer.at(i);

            bool didOverlap = false;
            int span = 2;
            for (int j = i - span; j < i; j++) {
                if (j > 0 && j < layer.size()) {
                    auto adjNode = layer.at(j);
                    if (i == j) {
                        continue;
                    }
                    didOverlap |= checkOverlap(vec2(node->getLocalLeft(), node->getLocalTop()), vec2(node->getLocalRight(), node->getLocalBottom()),
                                               vec2(adjNode->getLocalLeft(), adjNode->getLocalTop()), vec2(adjNode->getLocalRight(), adjNode->getLocalBottom()));
                }
            }
            if (didOverlap) {
                // find space
                for (int j = i; j < layer.size() - 1; j++) {

                    auto nodeJ = layer.at(j);
                    auto nextJ = layer.at(j + 1);

                    if (nodeJ == node || nextJ == node) {
                        continue;
                    }

                    int spaceSize = nodeJ->getLocalTop() - nextJ->getLocalBottom();
                    bool spaceFound = node->getMaxHeight() < spaceSize;

                    bool spaceFree = spaceTaken.count(nextJ->getLocalBottom()) == 0;
                    if (spaceFound && spaceFree) {
                        if (spaceTaken.count(nextJ->getLocalBottom()) == 0) {
                            spaceTaken.insert(nextJ->getLocalBottom());
                        }
                        node->setYOffset(nextJ->getLocalBottom() + 5);
                        break;
                    }
                }
            }
        }

        cout << "-----" << endl;
    }
}

vector<float> mUsedSpace;

void ZNodeEditor::adjustGraphLayout(ZNodeView *root, int depth) {
    if (depth == 0) {
        mUsedSpace.clear();
    }

    while (depth + 1 >= mUsedSpace.size()) {
        mUsedSpace.push_back(-1e7);
    }

    if (root->getChildren().size() > 0) {
        mUsedSpace.at(depth + 1) = std::max(mUsedSpace.at(depth + 1), (float) root->getChildren().at(0)->getOffsetY() - 5);
    }

    for (ZNodeView *child : root->getChildren()) {
        if (mUsedSpace.at(depth + 1) + 5 > child->getOffsetY() && depth != 0) {

            bool moveRoot = root->getOffsetY() + 10 > mUsedSpace.at(depth);
            offsetGraphBy(root, mUsedSpace.at(depth + 1) - child->getOffsetY() + 5, depth, true);
        }
        mUsedSpace.at(depth + 1) = std::max(mUsedSpace.at(depth + 1), (float) child->getLocalBottom());
        adjustGraphLayout(child, depth + 1);
    }
}

void ZNodeEditor::offsetGraphBy(ZNodeView *root, float y, int depth, bool moveRoot) {
    while (depth + 1 >= mUsedSpace.size()) {
        mUsedSpace.push_back(-1e7);
    }
    if (moveRoot) {
        root->offsetBy(0, y);
    }
    int i = 0;
    for (ZNodeView* child : root->getChildren()) {
        ZNodeView* nextSibling = nullptr;
        if (root->getChildren().size() < i + 1) {
            nextSibling = root->getChildren().at(i + 1);
        }
        offsetGraphBy(child, y, depth + 1, true);
        i++;
    }
}


float ZNodeEditor::centerGraph(ZNodeView *root, int depth) {

    float runningHeight = 0;
    float layerHeight = 0;
    for (ZNodeView* child : root->getChildren()) {
        layerHeight += child->getMaxHeight();
    }

    float offsetBy = 0;
    for (ZNodeView* child : root->getChildren()) {
        float margin = 5;
        float centerOffset = (layerHeight / 2) - child->getMaxHeight() / 2;
        float desiredOffset = runningHeight + root->getOffsetY() - centerOffset;
        child->setYOffset(runningHeight + root->getOffsetY() - centerOffset);
        float height = child->getMaxHeight();
        runningHeight += height + margin;
        offsetBy += centerGraph(child, depth + 1);
    }
    return 0;
}

void ZNodeEditor::selectNodeGraph(ZNodeView* root, int depth) {
    selectNode(root);
    for (auto parent : root->getChildren()) {
        selectNodeGraph(parent, depth + 1);
    }
}

void ZNodeEditor::selectNodeGraphInverse(ZNodeView* root, int depth) {
    selectNode(root);
    for (auto parent : root->getParents()) {
        selectNodeGraphInverse(parent, depth + 1);
    }
}

void ZNodeEditor::deleteNodeRecursive(ZNodeView* root) {
    if (root == nullptr) {
        return;
    }
    set<ZNodeView*> uniqueChildren;
    vector<ZNodeView*> children;
    vector<vector<pair<ZNodeView *, int>>> inputIndices = root->mInputIndices;

    for (const vector<pair<ZNodeView *, int>>& socketInputs : inputIndices) {
        // std::reverse(socketInputs.begin(), socketInputs.end());
        for (pair<ZNodeView*, int> input : socketInputs) {
            ZNodeView* child = input.first;

            if (child != nullptr) {
                if (uniqueChildren.count(child) == 0) {
                    uniqueChildren.insert(child);
                    children.push_back(child);
                }
            }
        }
    }

    for (ZNodeView* node : uniqueChildren) {
        deleteNodeRecursive(node);
    }

    deleteNodeAsync(root);
}

vector<string> ZNodeEditor::getNodeTypeNames(vector<NodeType*> types) {
    vector<string> names;
    for (NodeType* type : types) {
        names.push_back(type->mName);
    }

    return names;
}

void ZNodeEditor::onResume() {
    cout << "on resume" << endl;
}

void ZNodeEditor::onCreate() {
    ZView::onCreate();
    mEvaluateRunning = true;

}

void ZNodeEditor::onExit() {
    ZView::onExit();
    saveProject();
    mEvaluateRunning = false;
}

/**
 * Starts the evaluation background thread
 */
void ZNodeEditor::startEvaluation(ZNodeEditor* editor) {
    while(editor->mEvaluateRunning) {
        set<ZNodeView*> nodesToUpdate;
        bool wasDelete = false;
        while (!editor->mEvalSet.empty()) {
            {
                ZNodeView *node = *editor->mEvalSet.begin();

                if (!node->isDeleted()) {
                    nodesToUpdate.insert(node);
                } else {
                    editor->deleteNodeAsync(node);
                    wasDelete = true;
                }

                std::lock_guard<std::mutex> guard(editor->mEvalMutex);
                editor->mEvalSet.erase(node);
            }
        }

        if (editor->mLineContainer != nullptr) {
            for (ZNodeView* node : nodesToUpdate) {
                if (node->getIndexTag() != -1) {
                    if (node->getType()->mCompute != nullptr) {
                        node->updateChart();
                    }
                }
            }
        }

        glfwPostEmptyEvent();
        if (editor->mEvalSet.empty()) {
            {
                if (wasDelete) {
                    editor->updateLines();
                }

                if (editor->mSavePending) {
                    editor->saveProject();
                }

                std::unique_lock<std::mutex> lck(editor->mEvalMutex);
                editor->mEvalConVar.wait(lck);
            }
        }
    }
}

ZNodeView * ZNodeEditor::addNode(NodeType* type) {
    mLastType = type;
    ZNodeView* node = ZNodeUtil::get().newNode(type);
    addNodeToView(node, true);
    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
    return node;
}

void ZNodeEditor::addNodeToView(ZNodeView *node, bool autoPosition) {
    if (node->getParentView() != mNodeContainer) {
        mNodeContainer->addSubView(node);
        node->setInvalidateListener([this](ZNodeView* node){
            {
                lock_guard<mutex> guard(mEvalMutex);
                mEvalSet.insert(node);
                mEvalConVar.notify_one();

            }
        });
        node->setCornerRadius(5);
    }

    deselectNode(node);

    if (!node->mIsPartOfGroup || mGroupMode == GROUP_SELECTED) {
        node->setVisibility(true);
    } else {
        node->setVisibility(false);
    }

    if (node->getType()->mIsGroupNode) {
        for (ZNodeView *innerNode : node->getGroupNodes()) {
            innerNode->setVisibility(false);
        }
    }
    node->setIsDeleted(false);
    node->setIndexTag(mNodeViews.size());

    mNodeViews.push_back(node);
    vec2 nodeSize = node->getType()->mNodeSize;

    vec2 scale = mNodeContainer->getInnerView()->getScale();
    vec2 translation = mNodeContainer->getInnerView()->getInnerTranslation();

    if (autoPosition) {
        vec2 scrollOffset = mNodeContainer->getInnerView()->getTranslation();
        node->setOffset(mAddNodePosition - scrollOffset - translation);
        node->setInitialPosition((node->getOffset()));
        if (mAddNodePosition.x + NODE_MARGIN + nodeSize.x >= getWidth() / scale.x) {
            mAddNodePosition.x = DEFAULT_NODE_X;
            mAddNodePosition.y += nodeSize.y + NODE_MARGIN;
        } else {
            mAddNodePosition.x += nodeSize.x + NODE_MARGIN;
        }
    }

    node->setInitialPosition(node->getOffset() - getMouseDragDelta());

    node->setShowMagPickerListener([this, node](ZView *view, ZNodeView *nodeView,
            int index, bool isInput, float initialValue, string name) {
        vec2 mouse = getRelativeMouse();


        // This logic shows the popup slider window
        float difference = (mMagnitudePicker->getWidth()) / 2.0;

        double xpos = std::min(std::max(0.0, (double) (mouse.x - difference)),
                          (double) getWidth() - mMagnitudePicker->getMaxWidth());
        double ypos;

        double margin = 10;
        if (mouse.y - mMagnitudePicker->getMaxHeight() > 0) {
            // Show above
            ypos = mouse.y - (mMagnitudePicker->getMaxHeight() + margin);
            mMagnitudePicker->setShowAbove(true);
        } else {
            // Show below
            ypos = mouse.y + (view->getHeight() + margin);
            mMagnitudePicker->setShowAbove(false);
        }

        mMagnitudePicker->setInputType(index, isInput);
        mMagnitudePicker->setOffset(vec2(xpos, ypos));
        mMagnitudePicker->setVisibility(true);
        mMagnitudePicker->onWindowChange(getWindowWidth(), getWindowHeight());
        mMagnitudePicker->setTitle(name);
        mMagnitudePicker->setSliderVisibility(true);
        mMagnitudePicker->setRadioButtonNames(mRadioButtonNames);
        if (isInput) {
            mMagnitudePicker->selectMagnitude(node->getInputMagnitude(index));
        } else {
            mMagnitudePicker->selectMagnitude(node->getOutputMagnitude(index));
        }

        mMagnitudePicker->setValueChangedListener([this, nodeView](int index, float value, bool isInput, int magIndex) {
            if (!isInput) {
                nodeView->setConstantValue(index, value, magIndex);
            } else {
                nodeView->setConstantValueInput(index, value, magIndex);
            }

        });
        mMagnitudePicker->setValue(initialValue);
    });

    node->setShowEnumPickerListener([this, node](ZView *view, ZNodeView *nodeView,
                                                int index, bool isInput, float initialValue, string name,
                                                vector<string> enumNames) {
        vec2 mouse = getRelativeMouse();


        // This logic shows the popup slider window
        float difference = (mMagnitudePicker->getWidth()) / 2.0;

        double xpos = std::min(std::max(0.0, (double) (mouse.x - difference)),
                          (double) getWidth() - mMagnitudePicker->getMaxWidth());
        double ypos;

        double margin = 10;
        if (mouse.y - mMagnitudePicker->getMaxHeight() > 0) {
            // Show above
            ypos = mouse.y - (mMagnitudePicker->getMaxHeight() + margin);
            mMagnitudePicker->setShowAbove(true);
        } else {
            // Show below
            ypos = mouse.y + (view->getHeight() + margin);
            mMagnitudePicker->setShowAbove(false);
        }

        mMagnitudePicker->setInputType(index, isInput);
        mMagnitudePicker->setOffset(vec2(xpos, ypos));
        mMagnitudePicker->setVisibility(true);
        mMagnitudePicker->onWindowChange(getWindowWidth(), getWindowHeight());
        mMagnitudePicker->setTitle(name);
        mMagnitudePicker->setSliderVisibility(false);
        mMagnitudePicker->setRadioButtonNames(enumNames);

        if (isInput) {
            mMagnitudePicker->selectMagnitude(node->getInputMagnitude(index));
        } else {
            mMagnitudePicker->selectMagnitude(node->getOutputMagnitude(index));
        }

        mMagnitudePicker->setValueChangedListener([this, nodeView](int index, float value, bool isInput, int magIndex) {
            if (!isInput) {
                nodeView->setConstantValue(index, value, magIndex);
            } else {
                nodeView->setConstantValueInput(index, value, magIndex);
            }

        });
        mMagnitudePicker->setValue(initialValue);
    });
    node->resetInitialPosition();
    node->invalidateNodeRecursive();

    if (mGroupMode != NO_GROUP) {
        mNodeViews.at(mGroupMode)->addGroupNode(node);
    }

    node->setProjectID(mSelectedProject);
    node->setEditorInterface([this](ZNodeView* node, bool autoPosition){
        addNodeToView(node, autoPosition);
    });

    requestSave();
}

void ZNodeEditor::duplicateSelectedNodes(){
    enterGrabMode();
    auto duplicates = ZNodeUtil::get().duplicateNodes(mSelectedNodes);

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
    deselectAllNodes();

    for (ZNodeView* duplicate : duplicates) {
        selectNode(duplicate);
    }

    mDragNode = 1;
    invalidate();
}

void ZNodeEditor::deleteSelectedNodes() {
    for (ZNodeView* node : mSelectedNodes) {
        deleteNode(node);
    }

    resetCursor();
    mSelectedNodes.clear();

    if (!mExpressionField->isViewInFocus()) {
        mExpressionField->setText("");
    }
}

void ZNodeEditor::deleteSelectedConnections() {
    for (ZNodeView* node : mSelectedNodes) {
        deleteConnections(node);
    }
}

void ZNodeEditor::deleteNode(ZNodeView * node) {
    node->setIsDeleted(true);
    node->setVisibility(false);
    node->invalidateForDelete();
    reindexNodes();

}

void ZNodeEditor::deleteConnections(ZNodeView* node) {
    ZNodeUtil::get().deleteConnections(node);
}

void ZNodeEditor::deleteNodeAsync(ZNodeView *node) {// Otherwise remove the last added connection
    ZNodeUtil::get().deleteNode(node);
    mNodeListInvalid = true;


//    mNodeMutex.lock();
//    remove(mNodeViews, node->getIndexTag());
//    mNodeMutex.unlock();
}

void ZNodeEditor::reindexNodes() {
    int index = 0;
    for (int i = 0; i < mNodeViews.size(); i++) {
        if (mNodeViews.at(i)->getIndexTag() == -1) {
            remove(mNodeViews, i);
            i--;
        }
    }
    for (ZNodeView *nv : mNodeViews){
        nv->setIndexTag(index);
        index++;
    }
    mNodeListInvalid = false;
}

void ZNodeEditor::selectNode(ZNodeView* node) {
    if (node->getVisibility()) {
        node->setLineWidth(6.0);
        node->setOutlineColor(gold);
        node->setElevation(SELECTED_ELEVATION);
        node->bringToFront();

        mSecondLastSelected = mLastSelected;
        mLastSelected = node;
        mSelectedNodes.insert(node);

        if (!mExpressionField->isViewInFocus()) {
            if (mSelectedNodes.size() == 1) {
                mExpressionField->setText(ZNodeUtil::get().graphToExpString(node, true));
            } else {
                mExpressionField->setText("");
            }
        }
    }
}

void ZNodeEditor::deselectNode(ZNodeView* node) {
    if (node->getVisibility()) {
        node->setOutlineColor(transparent);
        node->setElevation(DEFAULT_ELEVATION);
    }

    if (mSelectedNodes.count(node) != 0) {
        mSelectedNodes.erase(node);
    }
}

void ZNodeEditor::deselectAllNodes() {
    vector<ZNodeView*> nodes;
    for (ZNodeView* oldNode : mSelectedNodes) {
        nodes.push_back(oldNode);
    }

    for (ZNodeView* oldNode : nodes) {
        deselectNode(oldNode);
    }

    if (!mExpressionField->isViewInFocus()) {
        mExpressionField->setText("");
    }
}

void ZNodeEditor::selectAllNodes() {
    for (ZNodeView* node : mNodeViews) {
        if(node->getVisibility()) {
            selectNode(node);
        }
    }
}

void ZNodeEditor::quickConnectNodes(ZNodeView* firstNode, ZNodeView* secondNode) {
    // Shortcut for linking two nodes. This works well when
    // the first node has the same number of outputs as the
    // second node has inputs.
    if (firstNode == secondNode || firstNode == nullptr || secondNode == nullptr) {
        return;
    }

    int minInputCount = 1e6;
    int minOutputCount = 1e6;

    int minInputIndex = 0;
    int minOutputIndex = 0;

    for (int i = 0; i < firstNode->getType()->mSocketCount.y; i++) {
        if (firstNode->mOutputIndices.at(i).size() < minOutputCount) {
            minOutputCount = firstNode->mOutputIndices.at(i).size();
            minOutputIndex = i;
        }
    }

    for (int i = 0; i < secondNode->getType()->mSocketCount.x; i++) {
        if (secondNode->mInputIndices.at(i).size() < minInputCount) {
            minInputCount = secondNode->mInputIndices.at(i).size();
            minInputIndex = i;
        }
    }

    connectNodes(minOutputIndex, minInputIndex, firstNode, secondNode);
}

void ZNodeEditor::connectNodes(int outIndex, int inIndex, ZNodeView *firstNode, ZNodeView *secondNode) const {
    firstNode->mOutputIndices.at(outIndex).push_back(pair<ZNodeView *, int>(secondNode, inIndex));
    secondNode->mInputIndices.at(inIndex).push_back(pair<ZNodeView *, int>(firstNode, outIndex));
    secondNode->invalidateNodeRecursive();
}

void ZNodeEditor::updateLines() {
    if (mNodeViews.empty()) {
        return;
    }

    for (ZLineView* lineView : mLineBucket) {
        lineView->setVisibility(false);
    }

    bool reindex = false;
    int lineIndex = 0;
    for (ZNodeView* node : mNodeViews) {
        if (node->getVisibility()) {
            int outputIndex = 0;
            vector<ZView *> outSockets = node->getSocketsOut();
            for (const vector<pair<ZNodeView *, int>> &nextNode : node->mOutputIndices) {
                if (!nextNode.empty()) {

                    for (pair<ZNodeView *, int> inputIndex : node->mOutputIndices.at(outputIndex)) {
                        if (inputIndex.first == nullptr) {
                            continue;
                        }
                        ZLineView *line = getLine(lineIndex++);
                        vec2 point = outSockets.at(outputIndex)->getCenter();
                        vec2 point2 =  inputIndex.first->getSocketsIn().at(inputIndex.second)->getCenter();

                        if (point.y <= mNodeContainer->getTop()) {
                            point.y = mNodeContainer->getTop() - 2;
                        }
                        if (point2.y <= mNodeContainer->getTop()) {
                            point2.y = mNodeContainer->getTop() - 2;
                        }

                        if (point.y >= mNodeContainer->getBottom() && point2.y >= mNodeContainer->getBottom()) {
                            line->setVisibility(false);
                        } else {
                            line->setVisibility(true);
                        }

                        line->setPoints(point,point2);
                    }
                }
                outputIndex++;
            }
        }
    }

    getParentView()->invalidate();

    if (mMagnitudePicker != nullptr) {
        mMagnitudePicker->invalidate();
    }
}

ZLineView* ZNodeEditor::getLine(int index) {
    if (mLineBucket.size() >= index + 1) {
        return mLineBucket.at(index);
    } else {
        ZLineView* line = new ZLineView(vec2(0), vec2(0), mLineContainer);
        mLineBucket.push_back(line);
        return mLineBucket.at(index);
    }
}

void ZNodeEditor::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) {
    ZView::onMouseDrag(absolute, start, delta, state, 0);


    if (middleMouseIsDown()) {
        mMagnitudePicker->setVisibility(false);
    }

    vec2 scale = mNodeContainer->getInnerView()->getRelativeScale();
    absolute /= scale;
    start /= scale;
    delta /= scale;


    if (state == mouseDown) {
        onMouseDown();
    } else if (state == mouseUp) {
        onMouseUp(button);
    }
    updateLines();
}

void ZNodeEditor::onMouseDown() {

    if (mBoxMode == BOX_SELECT) {
        enterBoxSelect2nd();
    }

    if (mouseIsDown()) {
        if (mGrab) {
            return;
        } else {
            enterGrabMode();
        }
    } else {
        mInitialOffset = getMouse();
    }

    bool mouseOverNode = false;
    int i = mNodeViews.size() - 1;
    for (auto it = mNodeViews.rbegin(); it != mNodeViews.rend(); ++it) {
        ZNodeView* node = (*it);
        if (isMouseInBounds(node) && node->getVisibility()) {
            mouseOverNode = true;
            mDragNode = i;

            // The check for right mouse down is to enable
            // the quick select feature to connect a single
            // node to multiple nodes easily.
            if (mBoxMode == NO_BOX_SELECT && !rightMouseIsDown()) {
                if (mSelectedNodes.count(node) == 0 && !shiftKeyPressed() &&
                    !clickConsumed()) {
                    deselectAllNodes();
                }

                if (mSelectedNodes.count(node) == 0) {
                    selectNode(node);
                } else if (shiftKeyPressed()){
                    // Toggle node selection when shift clicking node
                    mPendingDeselect = node;
                }
            }


            for (int j = 0; j < node->getSocketCount().x; j++) {
                ZView *socket = node->getSocketsIn().at(j);
                SocketType socketType = SocketType::CON;
                if (node->getType()->mSocketType.at(0).size() > j) {
                   socketType = node->getType()->mSocketType.at(0).at(j);
                }
                bool magPicker = (shiftKeyPressed() || socketType == SocketType::CON);

                if (isMouseInBounds(socket) && socket->getVisibility()) {

                    // If nothing connected show input line
                    if (node->mInputIndices.at(j).empty()) {
                        if (!magPicker) {
                            mDragType = SOCKET_DRAG_IN;
                            mInitialOffset = socket->getCenter();
                            mDragSocket = j;
                        }
                    } else {
                        // Otherwise remove the last added connection
                       pair<ZNodeView*, int> prevNode = node->mInputIndices.at(j).at(node->mInputIndices.at(j).size() - 1);
                       int k = 0;
                       if (prevNode.first != nullptr) {
                           for (pair<ZNodeView *, int> outputNode : prevNode.first->mOutputIndices.at(
                                   prevNode.second)) {
                               if (outputNode.first == node && outputNode.second == j) {
                                   remove(prevNode.first->mOutputIndices.at(prevNode.second), k);

                                   // Attach line to previous node after remove
                                   mDragType = SOCKET_DRAG_OUT;
                                   mInitialOffset = prevNode.first->getSocketsOut().at(prevNode.second)->getCenter();
                                   mDragSocket = prevNode.second;
                                   mDragNode = prevNode.first->getIndexTag();
                                   mTmpLine->setVisibility(true);
                                   mTmpLine->setPoints(mInitialOffset, node->getSocketsIn().at(j)->getCenter());
                                   break;
                               }
                               k++;
                           }
                       }
                       node->mInputIndices.at(j).pop_back();
                       node->invalidateNodeRecursive();
                       node->invalidate();
                    }
                }

            }

            int j = 0;
            for (ZView *socket : node->getSocketsOut()) {
                if (isMouseInBounds(socket) && socket->getVisibility()) {
                    mDragType = SOCKET_DRAG_OUT;
                    mInitialOffset = socket->getCenter();
                    mDragSocket = j;
                }
                j++;
            }

            if (!isSocketDrag()) {
                mDragType = NODE_DRAG;
                mInitialSize = vec2(node->getMaxWidth(), node->getMaxHeight());
            }
            break;
        }

        node->resetInitialPosition();
        i--;
    }

    if (!mouseOverNode && mouseIsDown() && !clickConsumed()) {
        deselectAllNodes();
    }

    updateLines();

}

bool ZNodeEditor::clickConsumed() const {
    return isMouseInBounds(mExpressionField) || isMouseInBounds(mButtonPanel);
}

void ZNodeEditor::onMouseMove(const vec2 &absolute, const vec2 &delta) {
    if (!mNodeViews.empty() && mDragNode != NO_SELECTION && mBoxMode == NO_BOX_SELECT) {
        if (isSocketDrag()) {
            mTmpLine->setVisibility(true);
            mTmpLine->setPoints(mInitialOffset, absolute);
        } else {
            if (mGrab && (!shiftKeyPressed() || !mouseIsDown())) {
                for (ZNodeView* selected : mSelectedNodes) {
                    selected->setOffset(
                            (int) selected->getInitialPosition().x + delta.x,
                            (int) selected->getInitialPosition().y + delta.y);
                    selected->onWindowChange(getWidth(), getHeight());
                    selected->invalidate();
                }

                updateLines();

            } else if (rightMouseIsDown() && !shiftKeyPressed()) {
                mNodeViews.at(mDragNode)->setMaxWidth(std::max(MIN_NODE_SIZE,
                        mInitialSize.x + delta.x));
                mNodeViews.at(mDragNode)->setMaxHeight(
                        std::max(MIN_NODE_SIZE, mInitialSize.y + delta.y));
                mNodeViews.at(mDragNode)->onWindowChange(getWidth(), getHeight());
                mNodeViews.at(mDragNode)->invalidate();

            }

            resetCursor();
        }
    }

    if ((middleMouseIsDown() && (!shiftKeyPressed() || mDragNode == NO_SELECTION)) ||
        ((altKeyPressed() && mDragNode == NO_SELECTION) && !isViewInFocus())) {
        for (ZNodeView* node : mNodeViews) {
            if (node->getVisibility()) {
                node->setOffset(
                        (int) node->getInitialPosition().x + delta.x,
                        (int) node->getInitialPosition().y + delta.y);
                node->onWindowChange(getWidth(), getHeight());
            }
        }

        resetCursor();
    }

    getRootView()->invalidate();
}

void ZNodeEditor::resetCursor() { mAddNodePosition = vec2(DEFAULT_NODE_X, DEFAULT_NODE_Y); }

void ZNodeEditor::onMouseUp(int button) {
    int nodeIndex = getMouseOverNode();

    if (isSocketDrag() && nodeIndex != NO_SELECTION) {
        int socketOutIndex = getMouseOverOutSocket(mNodeViews.at(nodeIndex));
        int socketInIndex = getMouseOverInSocket(mNodeViews.at(nodeIndex));

        ZNodeView *activeNode = mNodeViews.at(mDragNode);
        ZNodeView *targetNode = mNodeViews.at(nodeIndex);
        switch (mDragType) {
            case SOCKET_DRAG_IN:
                if (socketOutIndex != NO_SELECTION) {
                    int drag = mDragSocket;
                    connectNodes(socketOutIndex, drag, targetNode, activeNode);
                }
                break;
            case SOCKET_DRAG_OUT:
                if (socketInIndex != NO_SELECTION) {
                    connectNodes(mDragSocket, socketInIndex, activeNode, targetNode);
                }
                break;
            case NO_SELECTION:
                break;
            default:
                break;
        }
    }

    resetNodeInitialPosition();

    if (!isMouseInBounds(mExpressionField)) {
        releaseFocus(mExpressionField);
    }

    // If not shift select and user did not drag,
    // select the single drag node on mouse up.
    if (!wasMouseDrag() && !shiftKeyPressed() && nodeIndex != -1 &&
        button != GLFW_MOUSE_BUTTON_2 && !mWasDoubleClick) {
        deselectAllNodes();
        selectNode(mNodeViews.at(nodeIndex));
    }

    // Toggle node selection when shift key pressed
    if (mPendingDeselect != nullptr && !wasMouseDrag() && !mWasDoubleClick) {
        deselectNode(mPendingDeselect);
    }
    mPendingDeselect = nullptr;

    mDragNode = NO_SELECTION;
    mDragType = NO_SELECTION;
    mDragSocket = NO_SELECTION;
    mTmpLine->setVisibility(false);

    exitGrabMode();
    if (mBoxMode == BOX_SELECT_2) {
        updateBoxSelect();
    }
    exitBoxSelectMode();
    mWasDoubleClick = false;

    requestSave();
}

void ZNodeEditor::resetNodeInitialPosition() {
    for (ZNodeView* node : mNodeViews) {
        node->resetInitialPosition();
    }
}

bool ZNodeEditor::wasMouseDrag() {
    vec2 mouseDelta = abs(mInitialOffset - getMouse());
    return mouseDelta.x > DRAG_THRESHOLD || mouseDelta.y > DRAG_THRESHOLD;
}

void ZNodeEditor::enterGrabMode() {
    mGrab = true;
    mInitialOffset = getMouse();
}

void ZNodeEditor::exitGrabMode() {
    mGrab = false;
}

int ZNodeEditor::getMouseOverOutSocket(ZNodeView* node) {
    int socketIndex = 0;
    for (ZView* socket : node->getSocketsOut()) {
        if (isMouseInBounds(socket)) {
            return socketIndex;
        }
        socketIndex++;
    }
    return NO_SELECTION;
}

int ZNodeEditor::getMouseOverInSocket(ZNodeView* node) {
    int socketIndex = 0;
    for (ZView* socket : node->getSocketsIn()) {
        if (isMouseInBounds(socket)) {
            return socketIndex;
        }
        socketIndex++;
    }
    return NO_SELECTION;
}

int ZNodeEditor::getMouseOverNode() {
    int i = 0;
    for(ZNodeView* node : mNodeViews) {
        if (isMouseInBounds(node) && node->getVisibility()) {
            return i;
        }
        i++;
    }
    return NO_SELECTION;
}

bool ZNodeEditor::isSocketDrag() {
    return mDragType == SOCKET_DRAG_OUT || mDragType == SOCKET_DRAG_IN;
}

void ZNodeEditor::toggleGroupSelection() {
    // Only one node selected
    if (mGroupMode == NO_GROUP) {
        // Enter group selection
        if (mSelectedNodes.size() == 1) {
            ZNodeView *node = (*mSelectedNodes.begin());
            if (node->getType()->mIsGroupNode) {
                mGroupMode = node->getIndexTag();
                setBackgroundColor(grey2);

                for (ZNodeView* globalNode : mNodeViews) {
                    globalNode->setVisibility(false);
                }

                for (ZNodeView* groupNode : node->getGroupNodes()) {
                    groupNode->setVisibility(true);
                }

                node->setVisibility(false);
                node->getGroupInput()->setVisibility(true);
                node->getGroupOutput()->setVisibility(true);

                node->getGroupInput()->invalidateNodeRecursive();
                node->getGroupOutput()->invalidateNodeRecursive();
                onWindowChange(getWindowWidth(), getWindowHeight());

            }
        }
    } else {
        // Exit group select mode
        ZNodeView* groupNode = mNodeViews.at(mGroupMode);

        setBackgroundColor(bg);
        groupNode->setVisibility(true);
        deselectAllNodes();

        for (ZNodeView* node : mNodeViews) {

            if (node->getProjectID() != mSelectedProject) {
                continue;
            }
            // Check if node is part of a group before showing
            if (node->getType()->mShowInDrawer &&
                !node->mIsPartOfGroup) {
                node->setVisibility(true);
                cout << "showing node: " << node->getType()->mName << endl;
            }

            if (node->getType()->mIsGroupNode) {
                node->getGroupInput()->setVisibility(false);
                node->getGroupOutput()->setVisibility(false);
            }
        }

        for (ZNodeView* innerNode : groupNode->getGroupNodes()) {
            innerNode->setVisibility(false);
            cout << "hide node: " << innerNode->getType()->mName << endl;
        }
        groupNode->setSocketCount(vec2(groupNode->getGroupInput()->getSocketCount().y,
                                       groupNode->getGroupOutput()->getSocketCount().x));
        selectNode(groupNode);
        groupNode->invalidateNodeRecursive();

        mGroupMode = NO_GROUP;

    }

    updateLines();
    onWindowChange(getWindowWidth(), getWindowHeight());
}

void ZNodeEditor::selectNodeGraphUnderMouse() {
    int nodeIndex = getMouseOverNode();
    if (nodeIndex != -1) {
        ZNodeView* mouseOverNode = mNodeViews.at(getMouseOverNode());
        if (!shiftKeyPressed()) {
            deselectAllNodes();
        }
        selectNodeGraph(mouseOverNode, 0);
    }
}

void ZNodeEditor::selectNodeGraphInverseUnderMouse() {
    int nodeIndex = getMouseOverNode();
    if (nodeIndex != -1) {
        ZNodeView* mouseOverNode = mNodeViews.at(getMouseOverNode());
        if (!shiftKeyPressed()) {
            deselectAllNodes();
        }
        selectNodeGraphInverse(mouseOverNode, 0);
    }
}

void ZNodeEditor::onDoubleClick(int x, int y) {
    ZView::onDoubleClick(x, y);
    if (controlKeyPressed()) {
        selectNodeGraphInverseUnderMouse();
    } else {
        selectNodeGraphUnderMouse();
    }
    mWasDoubleClick = true;
}

void ZNodeEditor::onScrollChange(double x, double y) {

    if (ZSettings::get().getWheelMode() == scroll) {
        if (isMouseInBounds(mProjectBrowser)) {
            mNodeContainer->setEnableScroll(false);
        } else {
            mNodeContainer->setEnableScroll(true);
        }

        ZView::onScrollChange(x, y);
        updateLines();

        vec2 scroll = mNodeContainer->getInnerView()->getTranslation();
        vec2 offset = mNodeContainer->getInnerView()->getScale() * vec2(-2.0) * scroll / vec2(mCheckerView->getWidth(), mCheckerView->getHeight());
        mCheckerView->getBackgroundImage()->setOffset(offset);
        mCheckerView->computeBounds();

    } else {
        ZView::onScrollChange(x, y);
        mNodeContainer->setEnableScroll(false);

        // Scrolling with shift key is used for zooming charts
        if (!shiftKeyPressed() && !isMouseInBounds(mDrawer) && !isMouseInBounds(mProjectBrowser)) {
            float maxScale = 0.1;
            float minScale = 1.0;
            float scaleDelta = 1.0 + (y / 5.0);
            ZView* innerView = mNodeContainer->getInnerView();
            vec2 originalScale = innerView->getRelativeScale();
            vec2 newScale = max(vec2(maxScale), min(vec2(minScale), originalScale * vec2(scaleDelta)));
            vec2 initialPos = innerView->getInnerTranslation();
            vec2 origin = vec2(mNodeContainer->getWidth() / 2, mNodeContainer->getHeight() / 2);

            vec2 scaled = ((initialPos - origin) * newScale) + origin;
            vec2 scaledZero = (initialPos * newScale);

            vec2 offset = scaled - scaledZero;

            innerView->setScale(newScale);
            mLineContainer->setScale(newScale);


            vec2 delta = (offset) / newScale;

            int margin = (int) ((float) NODE_CONTAINER_OFFSET / newScale.y);

            mCheckerView->getBackgroundImage()->setScale(newScale.x * GRID_SCALE);
            mCheckerView->getBackgroundImage()->setOffset(vec3(0));
            mCheckerView->computeBounds();

            if (!(newScale.y == 1.0 && originalScale.y == 1.0) &&
                !(newScale.y == maxScale && originalScale.y == maxScale)) {
                innerView->setInnerTranslation(delta);
            }
            mNodeContainer->onWindowChange(getWidth(), getHeight());

            updateLines();
            mAddNodePosition = vec2(DEFAULT_NODE_X, DEFAULT_NODE_Y);
            getParentView()->getParentView()->getParentView()->invalidate();

        }
    }
}

/**
 * Box select mode
 */
void ZNodeEditor::enterBoxSelectMode() {
    mBoxMode = BOX_SELECT;
    mCursorView->setVisibility(true);
    mBoxSelect->setVisibility(false);
    mCursorView->setPosition((getMouse()) / mNodeContainer->getInnerView()->getScale());
}

void ZNodeEditor::exitBoxSelectMode() {
    mBoxMode = NO_BOX_SELECT;
    mBoxSelect->setVisibility(false);
    mCursorView->setVisibility(false);
    invalidate();
}

void ZNodeEditor::enterBoxSelect2nd() {
    mBoxMode = BOX_SELECT_2;
    mCursorView->setVisibility(false);
    mBoxSelect->setVisibility(true);
    vec2 scrollOffset = mNodeContainer->getInnerTranslation();
    mBoxSelect->setOffset((getRelativeMouse()) / mNodeContainer->getInnerView()->getScale());
    mBoxSelect->setMaxWidth(0);
    mBoxSelect->setMaxHeight(0);
}

void ZNodeEditor::updateBoxSelect() {
    vec2 p1 = vec2(mBoxSelect->getLeft(), mBoxSelect->getTop());
    vec2 p2 = p1 + mBoxSelect->getSize();

    vec2 min1 = vec2(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
    vec2 max1 = vec2(std::max(p1.x, p2.x), std::max(p1.y, p2.y));

    for (ZNodeView* node : mNodeViews) {
        vec2 p3 = (vec2(node->getLeft(), node->getTop()));
        vec2 p4 = (p3 + node->getSize());

        vec2 min2  = vec2(std::min(p3.x, p4.x), std::min(p3.y, p4.y));
        vec2 max2  = vec2(std::max(p3.x, p4.x), std::max(p3.y, p4.y));

        bool xOverlap = max1.x >= min2.x && max2.x >= min1.x;
        bool yOverlap = max1.y >= min2.y && max2.y >= min1.y;

        if (xOverlap && yOverlap) {
            selectNode(node);
        }
    }
}

bool ZNodeEditor::checkOverlap(vec2 p1a, vec2 p2a, vec2 p1b, vec2 p2b) {
    vec2 min1 = vec2(std::min(p1a.x, p2a.x), std::min(p1a.y, p2a.y));
    vec2 max1 = vec2(std::max(p1a.x, p2a.x), std::max(p1a.y, p2a.y));

    vec2 min2  = vec2(std::min(p1b.x, p2b.x), std::min(p1b.y, p2b.y));
    vec2 max2  = vec2(std::max(p1b.x, p2b.x), std::max(p1b.y, p2b.y));

    bool xOverlap = max1.x >= min2.x && max2.x >= min1.x;
    bool yOverlap = max1.y >= min2.y && max2.y >= min1.y;

    return xOverlap && yOverlap;
}

void ZNodeEditor::onCursorPosChange(double x, double y) {
    ZView::onCursorPosChange(x, y);
    if (mGrab || anyMouseDown() || altKeyPressed()) {
        if (!mMagnitudePicker->getVisibility()) {
            onMouseMove(vec2(x, y) / mNodeContainer->getInnerView()->getScale(),
                    vec2(x - mInitialOffset.x, y - mInitialOffset.y) / mNodeContainer->getInnerView()->getScale());

            if (!anyMouseDown()) {
                updateLines();
            }
        }
    }

    // Draw giant giant cursor around mouse for box select mode
    if (mBoxMode == BOX_SELECT)  {
        mCursorView->setPosition(getMouse() / mNodeContainer->getInnerView()->getScale());
        mCursorView->onWindowChange(getWindowWidth(), getWindowHeight());
        mCursorView->getParentView()->invalidate();
        invalidate();
    } else if (mBoxMode == BOX_SELECT_2) {

        vec2 scrollOffset = mNodeContainer->getInnerTranslation();

        vec2 mouse = getRelativeMouse() / mNodeContainer->getInnerView()->getScale();
        int mouseX = (int) mouse.x + scrollOffset.x;
        int mouseY = (int) mouse.y + scrollOffset.y;
        mBoxSelect->setMaxWidth(mouseX - (int) mBoxSelect->getOffsetX());
        mBoxSelect->setMaxHeight(mouseY - (int) mBoxSelect->getOffsetY());
        mBoxSelect->setBackgroundColor(faded);
    }
}

void ZNodeEditor::setExpVisibility(bool visible) {
    mExpressionField->setVisibility(false);
}

template <typename T>
void ZNodeEditor::remove(std::vector<T>& vec, size_t pos){
    auto it = vec.begin();
    std::advance(it, pos);
    vec.erase(it);
}

void ZNodeEditor::onMouseEvent(int button, int action, int mods, int sx, int sy) {
    ZView::onMouseEvent(button, action, mods, sx, sy);

    // Quick connect nodes
    int nodeIndex = getMouseOverNode();
    if (!wasMouseDrag() && (button == GLFW_MOUSE_BUTTON_2 || button == GLFW_MOUSE_BUTTON_3) && action == GLFW_RELEASE && nodeIndex != -1 && shiftKeyPressed()) {
        if (mSecondLastSelected != nullptr) {
            quickConnectNodes(mSecondLastSelected, mNodeViews.at(nodeIndex));

        }
    }

}
