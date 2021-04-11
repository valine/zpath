#include <utility>

//
// Created by lukas on 8/4/20.
//

#include <ui/zlineview.h>
#include <ui/zlabel.h>
#include <ui/zbutton.h>
#include <ui/nodeview.h>
#include <ui/zdropdown.h>
#include <ui/zcheckbox.h>
#include <ui/zmagnitudepicker.h>
#include <thread>
#include <zconf.h>
#include <utils/zsettingsstore.h>
#include <utils/znodeutil.h>
#include <ui/ztextfield.h>
#include <ui/zdrawer.h>
#include <utils/zcornerrenderer.h>
#include "ui/znodeeditor.h"
#include "utils/casutil.h"
ZNodeEditor::ZNodeEditor(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight, parent) {
    setBackgroundColor(ZSettingsStore::get().getBackgroundColor());

    vector<string> allTypes;
    vector<vec4> allColors;
    for (int i = 0; i != ZNodeView::Type::LAST; i++) {
        auto type = static_cast<ZNodeView::Type>(i);
        allTypes.push_back(ZNodeView::getName(type));
    }
    for (int i = 0; i != ZNodeView::Type::LAST; i++) {
        auto type = static_cast<ZNodeView::Type>(i);
        allColors.push_back(ZNodeView::getNodeColor(type));
    }

    mNodeContainer = new ZView(fillParent, fillParent, this);
    mLineContainer = new ZView(fillParent, fillParent, this);
    mNodeContainer->setYOffset(NODE_CONTAINER_OFFSET);
    mNodeContainer->setXOffset(100);

    mTmpLine = new ZLineView(vec2(20, 20), vec2(200, 200), mLineContainer);
    mTmpLine->setVisibility(false);

    mBoxSelect = new ZView(100, 100, mLineContainer);
    mBoxSelect->setOutlineType(WireType::outline);
    mBoxSelect->setOutlineColor(ZSettingsStore::get().getHighlightColor());
    mBoxSelect->setVisibility(false);
    mBoxSelect->setAllowNegativeSize(true);

    mCursorView = new ZCursorView(mLineContainer);
    mCursorView->setVisibility(false);
    mCursorView->setClippingEnabled(false);

    auto* line = new ZLineView(vec2(0), vec2(0), mLineContainer);
    mLineBucket.push_back(line);


    mDrawer = new ZDrawer(this, allTypes, allColors);
    mDrawer->setMarginTop(25);
    mDrawer->setOnItemSelected([this, allTypes](int index){
        vec2 mousePosition = (getRelativeMouse() / mNodeContainer->getScale()) - mNodeContainer->getInnerTranslation();
        vec2 startPosition = (mousePosition) + vec2(mNodeContainer->getMarginTop() - mDrawer->getMaxWidth(), 0);
        // startPosition.x = std::max((int) mDrawer->getWidth(), (int) startPosition.x);
        startPosition.y -= 40;
        startPosition.x -= 35;
        auto type = static_cast<ZNodeView::Type>(index);
        ZNodeView* node = addNode(type);
        node->setOffset(startPosition);

        selectNode(node);
        mDragNode = node->getIndexTag();
        node->setInitialPosition(startPosition);
        enterGrabMode();

    });

    mDrawer->setOnItemClicked([this, allTypes](int index){
        auto type = static_cast<ZNodeView::Type>(index);
        addNode(type);
    });

    auto* headerBackground = new ZView(fillParent, 25, this);
    headerBackground->setBackgroundColor(lightGrey);

    mHeader = new ZView(fillParent, fillParent, this);

    auto* dropDown = new ZDropDown(100,800, allTypes, mHeader);
    dropDown->setOffset(0, 0);
    dropDown->setTitle("Nodes");
    dropDown->wrapTitle();
    dropDown->setDynamicTitle(false);
    dropDown->setOnItemChange([this](int index){
        auto type = static_cast<ZNodeView::Type>(index);
        addNode(type);
    });

    vector<ZNodeView::Type> complexTypes = {
            ZNodeView::Type::Z,
            ZNodeView::Type::SIN,
            ZNodeView::Type::COS,
            ZNodeView::Type::TAN,
            ZNodeView::Type::EXP,};
    auto* complexDropdown = new ZDropDown(100,800, getNodeTypeNames(complexTypes), mHeader);
    complexDropdown->setOffset(100, 0);
    complexDropdown->setTitle("Trig");
    complexDropdown->setOffset(dropDown->getLocalRight(), 0);
    complexDropdown->wrapTitle();
    complexDropdown->setDynamicTitle(false);
    complexDropdown->setOnItemChange([this, complexTypes](int index){
        ZNodeView::Type type = complexTypes.at(index);
        addNode(type);
    });

    auto* viewDropDown = new ZDropDown(100,800, {"Snap to Nodes"}, mHeader);
    viewDropDown->setOffset(complexDropdown->getLocalRight(), 0);
    viewDropDown->setTitle("View");
    viewDropDown->wrapTitle();
    viewDropDown->setDynamicTitle(false);
    viewDropDown->setOnItemChange([this](int index){
        // Center view
        if (index == 0) {
            snapViewToNodes();
        }
    });


    thread evalThread = thread(ZNodeEditor::startEvaluation, this);
    evalThread.detach();

    auto* buttonPanel = new ZLinearLayout(80, fillParent, this);
    buttonPanel->setGravity(topRight);
    buttonPanel->setYOffset(30);
    buttonPanel->setXOffset(5);

    mLaplaceBtn = new ZButton("Laplace", buttonPanel);
    float cr = (float) mLaplaceBtn->getMaxHeight() / 2;
    mLaplaceBtn->setMaxWidth(fillParent);
    mLaplaceBtn->setCornerRadius(vec4(cr,cr,0,0));
    mLaplaceBtn->setMaxHeight(25);
    mLaplaceBtn->setOnClick([this](){

        cout << mSelectedNodes.size() << endl;
        // Only works if one node selected
        if (mSelectedNodes.size() == 1) {
            ZNodeView *root = (*mSelectedNodes.begin());
            string exp = ZNodeUtil::get().graphToString(root);
            string laplace = "laplace(" + exp + ")";
            string result = replace(CasUtil::get().evaluate(laplace), "\n", "");
            string zResult = replace(result, "x", "z");
            string heatResult = "heat(" + zResult + ")";
            addNodeGraph(ZNodeUtil::get().stringToGraph(heatResult).at(0), vec2(10), 0);
        }
    });

    // Button example
    auto* addNodeBtn = new ZButton("Add node", buttonPanel);
    addNodeBtn->setMarginTop(1);
    addNodeBtn->setCornerRadius(vec4(0,0,cr,cr));
    addNodeBtn->setOnClick([this](ZView* btn){
        addNode(mLastType);
    });

    // Magnitude picker work
    mMagnitudePicker = new ZMagnitudePicker(this);
    mMagnitudePicker->setVisibility(false);

    mExpressionField = new ZTextField(this);
    mExpressionField->setGravity(Gravity::bottomLeft);
    mExpressionField->setBackgroundColor(white);
    mExpressionField->setMaxWidth(360);
    mExpressionField->setXOffset(mDrawer->getMaxWidth() + 10);
    mExpressionField->setOutlineType(WireType::outline);
    mExpressionField->setMaxHeight(20);
    mExpressionField->setMargin(vec4(2));
    mExpressionField->setElevation(1.0);
    mExpressionField->setMargin(vec4(20));
    mExpressionField->setOnReturn([this](string value) {
       // vector<ZNodeView*> evalNodes = ZNodeUtil::get().stringToGraph(value);
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

        mSelectedNodes.clear();

        vector<ZNodeView*> evalNodes = ZNodeUtil::get().stringToGraph(value);

        if (!evalNodes.empty()) {
            addNodeGraph(evalNodes.at(0), pos, 0);
        }

    });


   //da addTestNodes();

    // Test computer algebra system library
//    CasUtil::get().testCompute();

//    testCorners();


}

string ZNodeEditor::replace(std::string subject, const std::string& search,
                            const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}

void ZNodeEditor::testCorners() {
    for (int i = 0; i < 3; i++) {

        float rad = i + 10;
        auto corner = new ZView(200, 200, this);
        corner->setYOffset(60 + (i * (200 + BUTTON_MARGIN)));
        corner->setMarginLeft(120);
        corner->setBackgroundColor(blue);
        corner->setCornerRadius(5);
        vec4 radius = vec4(rad, rad, rad, 30);

        auto corner1 = new ZView(300, 100, this);
        corner1->setYOffset(60 + (i * (200 + BUTTON_MARGIN)));
        corner1->setMarginLeft(120 + corner->getLocalRight() + BUTTON_MARGIN);
        corner1->setBackgroundColor(green);
        corner1->setCornerRadius(radius);


        auto corner2 = new ZView(50, 50, this);
        corner2->setYOffset(60 + (i * (200 + BUTTON_MARGIN)));
        corner2->setMarginLeft(120 + corner->getLocalRight() + corner1->getLocalRight() + 2 * BUTTON_MARGIN);
        corner2->setBackgroundColor(red);
        corner2->setCornerRadius(radius);

    }
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
                                mNodeContainer->getHeight()) * mNodeContainer->getScale()) / vec2(2);
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
    root->invalidateSingleNode();

    if (depth == 0) {
        deselectAllNodes();
        selectNode(root);
    }

    set<ZNodeView*> uniqueChildren;
    vector<ZNodeView*> children;
    vector<vector<pair<ZNodeView *, int>>> inputIndices = root->mInputIndices;

    for (const vector<pair<ZNodeView *, int>>& socketInputs : inputIndices) {
       // std::reverse(socketInputs.begin(), socketInputs.end());
        for (pair<ZNodeView*, int> input : socketInputs) {
            ZNodeView* child = input.first;

            if (uniqueChildren.count(child) == 0) {
                uniqueChildren.insert(child);
                children.push_back(child);
            }
        }
    }

    int margin = 20;
    if (mTmpNodeOffsetYR.size() <= depth && !children.empty()) {
        mTmpNodeOffsetYR.push_back(position.y);
        mTmpNodes.push_back(vector<ZNodeView*>());
    }

    if (root->getOffsetX() < 0) {
        mTmpNodeOffsetX = std::min(mTmpNodeOffsetX, (int) position.x);
    }

    for (ZNodeView* node : children) {
        vec2 nodeSize = node->getNodeSize(node->getType());
        addNodeGraph(node, vec2(position.x, mTmpNodeOffsetYR.at(depth)) - (vec2(nodeSize.x, 0) + vec2(margin, 0)), depth + 1);
        mTmpNodeOffsetYR.at(depth) += nodeSize.y + margin;
        mTmpNodes.at(depth).push_back(node);

        if (node->getOffsetX() - margin < 0) {
            mTmpNodeOffsetX = std::min(mTmpNodeOffsetX, (int) node->getOffsetX() - margin);
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

        root->setYOffset(root->getOffsetY() - (yOffset));
        root->setXOffset(root->getOffsetX() - mTmpNodeOffsetX);
        root->setInitialPosition(root->getOffset());
        for (vector<ZNodeView*> layer : mTmpNodes) {
            if (!layer.empty()) {
                int span = layer.at(layer.size() - 1)->getLocalBottom() - layer.at(0)->getLocalTop();
                int center = layer.at(0)->getLocalTop() + (span / 2);
                int rootCenter = ((root->getLocalBottom() - root->getLocalTop()) / 2);
                int offset = (center - position.y) - rootCenter;
                for (ZNodeView *node : layer) {
                    node->setXOffset(node->getOffsetX() - mTmpNodeOffsetX);
                    node->setYOffset(node->getOffsetY() - (offset + yOffset));
                    node->setInitialPosition(node->getOffset());
                }
            }
        }

        getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
        updateLines();

    }

}

void ZNodeEditor::selectNodeGraph(ZNodeView* root, int depth) {
    selectNode(root);

    set<ZNodeView*> uniqueChildren;vector<vector<pair<ZNodeView *, int>>> inputIndices = root->mInputIndices;

    for (const vector<pair<ZNodeView *, int>>& socketInputs : inputIndices) {
        for (pair<ZNodeView*, int> input : socketInputs) {
            ZNodeView* child = input.first;

            if (uniqueChildren.count(child) == 0) {
                uniqueChildren.insert(child);
            }
        }
    }

    for (ZNodeView* node : uniqueChildren) {
        selectNodeGraph(node, depth + 1);
    }
}

void ZNodeEditor::deleteNodeRecursive(ZNodeView* root) {
    set<ZNodeView*> uniqueChildren;
    vector<ZNodeView*> children;
    vector<vector<pair<ZNodeView *, int>>> inputIndices = root->mInputIndices;

    for (const vector<pair<ZNodeView *, int>>& socketInputs : inputIndices) {
        // std::reverse(socketInputs.begin(), socketInputs.end());
        for (pair<ZNodeView*, int> input : socketInputs) {
            ZNodeView* child = input.first;

            if (uniqueChildren.count(child) == 0) {
                uniqueChildren.insert(child);
                children.push_back(child);
            }
        }
    }

    for (ZNodeView* node : uniqueChildren) {
        deleteNodeRecursive(node);
    }

    deleteNode(root);
}

vector<string> ZNodeEditor::getNodeTypeNames(vector<ZNodeView::Type> types) {
    vector<string> names;
    for (ZNodeView::Type type : types) {
        names.push_back(ZNodeView::getName(type));
    }

    return names;
}

void ZNodeEditor::addTestNodes() {

    ZNodeView *sin = addNode(ZNodeView::SIN);
    ZNodeView *exp = addNode(ZNodeView::COS);

    ZNodeView *laplace = addNode(ZNodeView::LAPLACE);

    connectNodes(0, 0, sin, laplace);
    connectNodes(0, 0, exp, laplace);

    ZNodeView *x = addNode(ZNodeView::X);
    ZNodeView *c = addNode(ZNodeView::C);
    ZNodeView *s = addNode(ZNodeView::SIN);
    ZNodeView *co = addNode(ZNodeView::COS);
    ZNodeView *chart0 = addNode(ZNodeView::CHART_2D);

    connectNodes(0, 0, c, s);
    connectNodes(0, 0, x, s);
    connectNodes(0, 0, x, co);
    connectNodes(0, 0, s, chart0);
    connectNodes(0, 1, co, chart0);

    ZNodeView *lastChart = chart0;
    for (int i = 0; i < 2; i++) {
        ZNodeView *chart1 = addNode(ZNodeView::CHART_2D);
        connectNodes(0, 0, lastChart, chart1);
        connectNodes(1, 1, lastChart, chart1);
        lastChart = chart1;
    }

    updateLines();
}

void ZNodeEditor::onCreate() {
    ZView::onCreate();

}

void ZNodeEditor::onLayoutFinished() {
    ZView::onLayoutFinished();
}

/**
 * Starts the evaluation background thread
 */
void ZNodeEditor::startEvaluation(ZNodeEditor* editor) {

    // Todo: end loop when window closes
    bool shouldRun = true;

    while(shouldRun) {
        set<ZNodeView*> nodesToUpdate;
        while (!editor->mEvalSet.empty()) {
            {
                ZNodeView *node = *editor->mEvalSet.begin();

                if (node->getVisibility()) {
                    nodesToUpdate.insert(node);
                } else {
                    editor->deleteNodeAsync(node);
                    editor->updateLines();
                }

                std::lock_guard<std::mutex> guard(editor->mEvalMutex);
                editor->mEvalSet.erase(node);
            }
        }

        if (editor->mLineContainer != nullptr) {
            for (ZNodeView* node : nodesToUpdate) {
                node->updateChart();
            }
        }

        glfwPostEmptyEvent();
        {
            std::unique_lock<std::mutex> lck(editor->mEvalMutex);
            editor->mEvalConVar.wait(lck);
        }
    }
}

ZNodeView * ZNodeEditor::addNode(ZNodeView::Type type) {
    mLastType = type;
    vec2 nodeSize = ZNodeView::getNodeSize(type);

    ZNodeView* node = ZNodeUtil::get().newNode(type);
    addNodeToView(node, true);

    getRootView()->onWindowChange(getWindowWidth(), getWindowHeight());
    return node;
}

void ZNodeEditor::addNodeToView(ZNodeView *node, bool autoPosition) {
    if (node->getParentView() != mNodeContainer) {
        mNodeContainer->addSubView(node);
    }
    deselectNode(node);
    node->setCornerRadius(5);
    node->setVisibility(true);
    mNodeViews.push_back(node);
    vec2 nodeSize = ZNodeView::getNodeSize(node->getType());
    node->setMaxWidth(nodeSize.x);
    node->setIndexTag(mNodeViews.size() - 1);

    vec2 scale = mNodeContainer->getScale();
    vec2 translation = mNodeContainer->getInnerTranslation();

    if (autoPosition) {
        node->setOffset(mAddNodePosition - translation);
        node->setInitialPosition((node->getOffset()));
        if (mAddNodePosition.x + NODE_MARGIN + nodeSize.x >= getWidth() / scale.x) {
            mAddNodePosition.x = DEFAULT_NODE_X;
            mAddNodePosition.y += nodeSize.y + NODE_MARGIN;
        } else {
            mAddNodePosition.x += nodeSize.x + NODE_MARGIN;
        }
    }

    node->setInitialPosition(node->getOffset() - getMouseDragDelta());
    node->setInvalidateListener([this](ZNodeView* node){
        {
            lock_guard<mutex> guard(mEvalMutex);
            mEvalSet.insert(node);
            mEvalConVar.notify_one();

        }
    });

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
    node->invalidateSingleNode();
}

void ZNodeEditor::duplicateSelectedNodes(){

    map<ZNodeView*, ZNodeView*> tmpMap;
    vector<ZNodeView*> originals;
    vector<ZNodeView*> duplicates;
    enterGrabMode();
    for (ZNodeView* node : mSelectedNodes) {
        ZNodeView* dupNode = addNode(node->getType());
        dupNode->copyParameters(node);

        dupNode->resetInitialPosition();
        dupNode->invalidate();

        originals.push_back(node);
        duplicates.push_back(dupNode);

        tmpMap.insert(pair<ZNodeView*, ZNodeView*>(node, dupNode));
    }

    for (ZNodeView* original : originals) {
        ZNodeView* duplicate = tmpMap.at(original);

        int outputIndex = 0;
        // Looping over sockets
        for (const vector<pair<ZNodeView*, int>>& edges : original->mOutputIndices) {
            // Looping over individual edges
            for(pair<ZNodeView*, int> edge : edges) {
                ZNodeView* nextNode = edge.first;
                int inputIndex = edge.second;

                if (tmpMap.find(nextNode) != tmpMap.end()) {
                    connectNodes(outputIndex, inputIndex, duplicate, tmpMap.at(nextNode));
                }
            }
            outputIndex++;
        }
    }

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
}

void ZNodeEditor::deleteSelectedConnections() {
    for (ZNodeView* node : mSelectedNodes) {
        deleteConnections(node);
    }

    updateLines();
}

void ZNodeEditor::deleteNode(ZNodeView * node) {
    node->invalidateSingleNode();
    node->setVisibility(false);
}

void ZNodeEditor::deleteConnections(ZNodeView* node) {
        for (const vector<pair<ZNodeView *, int>> &inputs : node->mInputIndices) {
            for (pair<ZNodeView *, int> input : inputs) {
                ZNodeView *prevNode = input.first;
                int index = 0;
                for (pair<ZNodeView *, int> outputNode: prevNode->mOutputIndices.at(input.second)) {
                    if (outputNode.first == node) {
                        remove(prevNode->mOutputIndices.at(input.second), index);
                    }

                    index++;
                }
            }
        }

        for (const vector<pair<ZNodeView *, int>> &outputs : node->mOutputIndices) {
            int pairIndex = 0;
            for (pair<ZNodeView *, int> output : outputs) {
                ZNodeView *nextNode = output.first;

                if (nextNode != nullptr) {
                    remove(nextNode->mInputIndices.at(output.second), pairIndex);
                    nextNode->invalidateNodeRecursive();
                }
                pairIndex++;

            }
        }

        node->initializeEdges();
        node->invalidateSingleNode();
}

void ZNodeEditor::deleteNodeAsync(ZNodeView *node) {// Otherwise remove the last added connection
    node->setVisibility(false);
    deleteConnections(node);
    remove(mNodeViews, node->getIndexTag());
    int index = 0;
    for (ZNodeView *nv : mNodeViews){
        nv->setIndexTag(index);
        index++;
    }

   // mNodeContainer->removeSubView(node);
    ZNodeUtil::get().submitForRecycle(node);
    // Reindex node views
    invalidate();
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
                mExpressionField->setText(ZNodeUtil::get().graphToString(node));
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
        if (mSelectedNodes.count(node) != 0) {
            mSelectedNodes.erase(node);
        }
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

    for (int i = 0; i < firstNode->getSocketCount().y; i++) {
        if (firstNode->mOutputIndices.at(i).size() < minOutputCount) {
            minOutputCount = firstNode->mOutputIndices.at(i).size();
            minOutputIndex = i;
        }
    }

    for (int i = 0; i < secondNode->getSocketCount().x; i++) {
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
    for (ZLineView* lineView : mLineBucket) {
        lineView->setVisibility(false);
    }

    int lineIndex = 0;
    for (ZNodeView* node : mNodeViews) {
        if (node->getVisibility()) {
            int outputIndex = 0;
            vector<ZView *> outSockets = node->getSocketsOut();
            for (const vector<pair<ZNodeView *, int>> &nextNode : node->mOutputIndices) {
                if (!nextNode.empty()) {

                    for (pair<ZNodeView *, int> inputIndex : node->mOutputIndices.at(outputIndex)) {
                        ZLineView *line = getLine(lineIndex++);
                        line->setPoints(outSockets.at(outputIndex)->getCenter(),
                                        inputIndex.first->getSocketsIn().at(inputIndex.second)->getCenter());
                        line->setVisibility(true);
                    }
                }
                outputIndex++;
            }
        }
    }
    getParentView()->invalidate();
    mMagnitudePicker->invalidate();
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

    vec2 scale = mNodeContainer->getRelativeScale();
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
                bool magPicker = (shiftKeyPressed() || node->getSocketType().at(0).at(j) == ZNodeView::SocketType::CON);

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
                       for (pair<ZNodeView*, int> outputNode : prevNode.first->mOutputIndices.at(prevNode.second)) {
                           if (outputNode.first == node && outputNode.second == j) {
                               remove(prevNode.first->mOutputIndices.at(prevNode.second), k);

                               // Attach line to previous node after remove
                               mDragType = SOCKET_DRAG_OUT;
                               mInitialOffset = prevNode.first->getSocketsOut().at(prevNode.second)->getCenter();
                               mDragSocket = prevNode.second;
                               mDragNode = prevNode.first->getIndexTag();
                               mTmpLine->setVisibility(true);
                               mTmpLine->setPoints(node->getSocketsIn().at(j)->getCenter(), mInitialOffset);
                               break;
                           }
                           k++;
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
    return isMouseInBounds(mExpressionField) || isMouseInBounds(mLaplaceBtn);
}

void ZNodeEditor::onMouseMove(const vec2 &absolute, const vec2 &delta) {
    if (!mNodeViews.empty() && mDragNode != NO_SELECTION && mBoxMode == NO_BOX_SELECT) {
        if (isSocketDrag()) {
            mTmpLine->setVisibility(true);
            mTmpLine->setPoints(absolute, mInitialOffset);
        } else {
            if (mGrab && !shiftKeyPressed()) {
                for (ZNodeView* selected : mSelectedNodes) {
                    selected->setOffset(
                            (int) selected->getInitialPosition().x + delta.x,
                            (int) selected->getInitialPosition().y + delta.y);
                    selected->onWindowChange(getWidth(), getHeight());
                    selected->invalidate();
                }

            } else if (rightMouseIsDown() && !shiftKeyPressed()) {
                mNodeViews.at(mDragNode)->setMaxWidth(
                        mInitialSize.x + delta.x);
                mNodeViews.at(mDragNode)->setMaxHeight(
                        mInitialSize.y + delta.y);
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

void ZNodeEditor::onKeyPress(int key, int scancode, int action, int mods) {
    ZView::onKeyPress(key, scancode, action, mods);
    if (key == GLFW_KEY_R && shiftKeyPressed() && action == GLFW_PRESS) {
        addNode(mLastType);
    }

    else if (key == GLFW_KEY_E && shiftKeyPressed()) {
        addNode(mLastType);
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

void ZNodeEditor::onDoubleClick() {
    ZView::onDoubleClick();
    selectNodeGraphUnderMouse();
    mWasDoubleClick = true;
}

void ZNodeEditor::onScrollChange(double x, double y) {
    ZView::onScrollChange(x, y);

    // Scrolling with shift key is used for zooming charts
    if (!shiftKeyPressed() && !isMouseInBounds(mDrawer)){
        float maxScale = 0.3;
        float minScale = 1.0;
        float scaleDelta = 1.0 + (y / 5.0);
        vec2 originalScale = mNodeContainer->getRelativeScale();
        vec2 newScale = max(vec2(maxScale), min(vec2(minScale), originalScale * vec2(scaleDelta)));


        vec2 initialPos = mNodeContainer->getInnerTranslation();
        vec2 origin = vec2(getWidth() / 2, getHeight() / 2);

        vec2 scaled = ((initialPos - origin) * newScale) + origin;
        vec2 scaledZero = (initialPos * newScale);

        vec2 offset = scaled - scaledZero;

        mNodeContainer->setScale(newScale);
        mLineContainer->setScale(newScale);

        vec2 delta = (offset) / newScale;

        int margin = (int) ((float) NODE_CONTAINER_OFFSET / newScale.y);

        if (!(newScale.y == 1.0 && originalScale.y == 1.0) &&
                !(newScale.y == maxScale && originalScale.y == maxScale)) {
            mNodeContainer->setYOffset(margin);
            mNodeContainer->setInnerTranslation(delta);
        }
        mNodeContainer->onWindowChange(getWidth(), getHeight());

        updateLines();
        mAddNodePosition = vec2(DEFAULT_NODE_X, DEFAULT_NODE_Y);
        getParentView()->getParentView()->getParentView()->invalidate();

    }
}

/**
 * Box select mode
 */
void ZNodeEditor::enterBoxSelectMode() {
    mBoxMode = BOX_SELECT;
    mCursorView->setVisibility(true);
    mBoxSelect->setVisibility(false);
    mCursorView->setPosition(getMouse() / mNodeContainer->getScale());
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
    mBoxSelect->setOffset(getRelativeMouse() / mNodeContainer->getScale());
    mBoxSelect->setMaxWidth(0);
    mBoxSelect->setMaxHeight(0);
}

void ZNodeEditor::updateBoxSelect() {
    vec2 p1 = mBoxSelect->getOffset();
    vec2 p2 = p1 + mBoxSelect->getSize();

    vec2 min1 = vec2(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
    vec2 max1 = vec2(std::max(p1.x, p2.x), std::max(p1.y, p2.y));

    for (ZNodeView* node : mNodeViews) {
        vec2 p3 = node->getOffset() + mNodeContainer->getOffset();
        vec2 p4 = p3 + node->getSize();

        vec2 min2  = vec2(std::min(p3.x, p4.x), std::min(p3.y, p4.y)) +
                mNodeContainer->getInnerTranslation();
        vec2 max2  = vec2(std::max(p3.x, p4.x), std::max(p3.y, p4.y)) +
                mNodeContainer->getInnerTranslation();

        bool xOverlap = max1.x >= min2.x && max2.x >= min1.x;
        bool yOverlap = max1.y >= min2.y && max2.y >= min1.y;

        if (xOverlap && yOverlap) {
            selectNode(node);
        }
    }
}

void ZNodeEditor::onCursorPosChange(double x, double y) {
    ZView::onCursorPosChange(x, y);
    if (mGrab || anyMouseDown() || altKeyPressed()) {
        if (!mMagnitudePicker->getVisibility()) {
            onMouseMove(vec2(x, y) / mNodeContainer->getScale(),
                    vec2(x - mInitialOffset.x, y - mInitialOffset.y) / mNodeContainer->getScale());

            if (!anyMouseDown()) {
                updateLines();
            }
        }
    }

    // Draw giant giant cursor around mouse for box select mode
    if (mBoxMode == BOX_SELECT)  {
        mCursorView->setPosition(getMouse() / mNodeContainer->getScale());
        mCursorView->onWindowChange(getWindowWidth(), getWindowHeight());
        mCursorView->getParentView()->invalidate();
        invalidate();
    } else if (mBoxMode == BOX_SELECT_2) {

        vec2 mouse = getRelativeMouse() / mNodeContainer->getScale();
        int mouseX = (int) mouse.x;
        int mouseY = (int) mouse.y;
        mBoxSelect->setMaxWidth(mouseX - (int) mBoxSelect->getOffsetX());
        mBoxSelect->setMaxHeight(mouseY - (int) mBoxSelect->getOffsetY());
        mBoxSelect->setBackgroundColor(faded);
    }
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
    if (!wasMouseDrag() && (button == GLFW_MOUSE_BUTTON_2 || button == GLFW_MOUSE_BUTTON_3) && action == GLFW_RELEASE && nodeIndex != -1) {
        if (mSecondLastSelected != nullptr) {
            quickConnectNodes(mSecondLastSelected, mNodeViews.at(nodeIndex));

        }
    }

}
