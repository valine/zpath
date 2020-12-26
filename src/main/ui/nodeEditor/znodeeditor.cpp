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
#include "ui/znodeeditor.h"

ZNodeEditor::ZNodeEditor(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight, parent) {
    setBackgroundColor(vec4(0.005130, 0.013321, 0.025381, 1.000000));

    mNodeContainer = new ZView(fillParent, fillParent, this);
    mLineContainer = new ZView(fillParent, fillParent, this);
    mNodeContainer->setYOffset(NODE_CONTAINER_OFFSET);
    
    mHeader = new ZView(fillParent, fillParent, this);

    mTmpLine = new ZLineView(vec2(20, 20), vec2(200, 200), mLineContainer);
    mTmpLine->setVisibility(false);

    mBoxSelect = new ZView(100, 100, mLineContainer);
    mBoxSelect->setOutlineType(WireType::outline);
    mBoxSelect->setOutlineColor(ZSettingsStore::get().getHighlightColor());
    mBoxSelect->setVisibility(false);
    mBoxSelect->setAllowNegativeSize(true);

    mCursorView = new ZCursorView(mLineContainer);

    ZLineView* line = new ZLineView(vec2(0), vec2(0), mLineContainer);
    mLineBucket.push_back(line);
    // Button example
    auto* addNodeBtn = new ZButton("Add node", mHeader);
    addNodeBtn->setOnClick([this](ZView* btn){
        addNode(mLastType);
    });

    vector<string> allTypes;
    for (int i = 0; i != ZNodeView::Type::LAST; i++) {
        auto type = static_cast<ZNodeView::Type>(i);
        allTypes.push_back(ZNodeView::getName(type));
    }

    ZDropDown* dropDown = new ZDropDown(100,700, allTypes, mHeader);
    dropDown->setOffset(150, 0);
    dropDown->setTitle("Node Picker");

    dropDown->setOnItemChange([this](int index){
        auto type = static_cast<ZNodeView::Type>(index);
        addNode(type);
    });

    thread evalThread = thread(ZNodeEditor::startEvaluation, this);
    evalThread.detach();

    // Magnitude picker work
    mMagnitudePicker = new ZMagnitudePicker(this);
    mMagnitudePicker->setVisibility(false);
}

void ZNodeEditor::onLayoutFinished() {
    ZView::onLayoutFinished();
    addTestNodes();
}

void ZNodeEditor::addTestNodes() {
        ZNodeView *sin = addNode(ZNodeView::SIN);
        ZNodeView *fft = addNode(ZNodeView::FFT);
//    ZNodeView *x = addNode(ZNodeView::X);
//    ZNodeView *c = addNode(ZNodeView::C);
//    ZNodeView *s = addNode(ZNodeView::SIN);
//    ZNodeView *co = addNode(ZNodeView::COS);
//    ZNodeView *chart0 = addNode(ZNodeView::CHART_2D);
//
//    connectNodes(0, 0, c, s);
//    connectNodes(0, 0, x, s);
//
//    connectNodes(0, 0, x, co);
//
//    connectNodes(0, 0, s, chart0);
//    connectNodes(0, 1, co, chart0);
//
//    ZNodeView *lastChart = chart0;
//    for (int i = 0; i < 7; i++) {
//        ZNodeView *chart1 = addNode(ZNodeView::CHART_2D);
//        connectNodes(0, 0, lastChart, chart1);
//        connectNodes(1, 1, lastChart, chart1);
//        lastChart = chart1;
//    }

    updateLines();
}

/**
 * Starts the evaluation background thread
 */
void ZNodeEditor::startEvaluation(ZNodeEditor* editor) {

    // Todo: end loop when window closes
    bool shouldRun = true;

    while(shouldRun) {
        while (!editor->mEvalSet.empty()) {

            ZNodeView *node = *editor->mEvalSet.begin();
            if (node->getVisibility()) {
                std::lock_guard<std::mutex> guard(editor->mEvalMutex);
                editor->mEvalSet.erase(node);

                node->updateChart();
            } else {
                editor->removeNodeAsync(node);

                std::lock_guard<std::mutex> guard(editor->mEvalMutex);
                editor->mEvalSet.erase(node);
                node->removeView();
                editor->updateLines();
            }

            editor->mLineContainer->invalidate();
            editor->mMagnitudePicker->invalidate();
        }
        glfwPostEmptyEvent();
        std::unique_lock<std::mutex> lck(editor->mEvalMutex);
        editor->mEvalConVar.wait(lck);
    }
}

ZNodeView * ZNodeEditor::addNode(ZNodeView::Type type) {
    mLastType = type;

    vec2 nodeSize = ZNodeView::getNodeSize(type);
    auto* node = new ZNodeView(nodeSize.x, nodeSize.y, mNodeContainer);
    mNodeViews.push_back(node);

    node->setIndexTag(mNodeViews.size() - 1);

    vec2 scale = mNodeContainer->getScale();
    vec2 translation = mNodeContainer->getInnerTranslation();

    node->setOffset(mAddNodePosition - translation);
    node->setInitialPosition((mAddNodePosition - translation) - getMouseDragDelta());

    if (mAddNodePosition.x + NODE_MARGIN + nodeSize.x >= getWidth() / scale.x) {
        mAddNodePosition.x = DEFAULT_NODE_X;
        mAddNodePosition.y += nodeSize.y + NODE_MARGIN;
    } else {
        mAddNodePosition.x += nodeSize.x + NODE_MARGIN;
    }

    node->setType(type);
    node->onWindowChange(getWidth(), getHeight());

    node->setInvalidateListener([this](ZNodeView* node){
        std::lock_guard<std::mutex> guard(mEvalMutex);
        mEvalSet.insert(node);
        mEvalConVar.notify_one();
    });

    node->setShowMagPickerListener([this, node](ZView *view, ZNodeView *nodeView,
            int index, bool isInput, float initialValue, string name) {
        vec2 mouse = getMouse();

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
    return node;
}

void ZNodeEditor::duplicateSelectedNodes(){

    map<ZNodeView*, ZNodeView*> tmpMap;
    vector<ZNodeView*> originals;
    vector<ZNodeView*> duplicates;
    enterGrabMode();
    for (ZNodeView* node : mSelectedNodes) {
        ZNodeView* dupNode = addNode(node->getType());
        dupNode->setOffset(node->getOffset());
        dupNode->setMaxWidth(node->getMaxWidth());
        dupNode->setMaxHeight(node->getMaxHeight());
        dupNode->onWindowChange(getWidth(), getHeight());
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
        for (vector<pair<ZNodeView*, int>> edges : original->mOutputIndices) {
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
    node->setVisibility(false);
    node->invalidateSingleNode();
    updateLines();
}

void ZNodeEditor::deleteConnections(ZNodeView* node) {
    for (vector<pair<ZNodeView *, int>> inputs : node->mInputIndices) {
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

    for (vector<pair<ZNodeView *, int>> outputs : node->mOutputIndices) {
        int pairIndex = 0;
        for (pair<ZNodeView *, int> output : outputs) {
            ZNodeView *nextNode = output.first;
            remove(nextNode->mInputIndices.at(output.second), pairIndex);
            nextNode->invalidateNodeRecursive();
            pairIndex++;
        }
    }

    node->initializeEdges();
    node->invalidateSingleNode();
}

void ZNodeEditor::removeNodeAsync(ZNodeView *node) {// Otherwise remove the last added connection
    deleteConnections(node);
    remove(mNodeViews, node->getIndexTag());

    // Reindex node views
    int index = 0;
    for (ZNodeView *nv : mNodeViews) {
        nv->setIndexTag(index);
        index++;
    }

    invalidate();
}

void ZNodeEditor::selectNode(ZNodeView* node) {
    if (node->getVisibility()) {
        node->setOutlineColor(yellow);
        node->setLineWidth(3.0);

        mSecondLastSelected = mLastSelected;
        mLastSelected = node;
        mSelectedNodes.insert(node);

    }
}

void ZNodeEditor::deselectNode(ZNodeView* node) {
    node->setOutlineColor(grey);
    node->setLineWidth(2.0);
    if (mSelectedNodes.count(node) != 0) {
        mSelectedNodes.erase(node);
    }
}

void ZNodeEditor::deselectAllNodes() {
    for (ZNodeView* oldNode : mSelectedNodes) {
        deselectNode(oldNode);
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

    mMouseDragDelta = absolute - start;

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
    int i = 0;
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
    for (ZNodeView *node : mNodeViews) {
        if (isMouseInBounds(node) && node->getVisibility()) {
            mouseOverNode = true;
            mDragNode = i;

            // The check for right mouse down is to enable
            // the quick select feature to connect a single
            // node to multiple nodes easily.
            if (mBoxMode == NO_BOX_SELECT && !rightMouseIsDown()) {
                if (mSelectedNodes.count(node) == 0 && !shiftKeyPressed()) {
                    deselectAllNodes();
                }
                selectNode(node);
            }


            for (int j = 0; j < node->getSocketCount().x; j++) {
                ZView *socket = node->getSocketsIn().at(j);
                bool magPicker = (shiftKeyPressed() || node->getSocketType().first.at(j) == ZNodeView::SocketType::CON);

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
                        //
                        //
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
                mInitialSize = vec2(node->getWidth(), node->getHeight());
            }
            break;
        }

        node->resetInitialPosition();
        i++;
    }

    if (!mouseOverNode && mouseIsDown()) {
        deselectAllNodes();
    }

    updateLines();

}

void ZNodeEditor::onMouseMove(const vec2 &absolute, const vec2 &delta) {
    if (!mNodeViews.empty() && mDragNode != NO_SELECTION && mBoxMode == NO_BOX_SELECT) {
        if (isSocketDrag()) {
            mTmpLine->setVisibility(true);
            mTmpLine->setPoints(absolute, mInitialOffset);
        } else {
            if (mGrab) {
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

    if (middleMouseIsDown() && (!shiftKeyPressed() || mDragNode == NO_SELECTION)) {
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

    getParentView()->getParentView()->getParentView()->invalidate();
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

    for (ZNodeView* node : mNodeViews) {
        node->resetInitialPosition();
    }

    // If not shift select and user did not drag,
    // select the single drag node on mouse up.
    if (!wasMouseDrag() && !shiftKeyPressed() && nodeIndex != -1 && button != GLFW_MOUSE_BUTTON_2) {
        deselectAllNodes();
        selectNode(mNodeViews.at(nodeIndex));
    }

    mDragNode = NO_SELECTION;
    mDragType = NO_SELECTION;
    mDragSocket = NO_SELECTION;
    mTmpLine->setVisibility(false);

    exitGrabMode();
    if (mBoxMode == BOX_SELECT_2) {
        updateBoxSelect();
    }

    mMouseDragDelta = vec2(0);
    exitBoxSelectMode();
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

    else if (key == GLFW_KEY_D && shiftKeyPressed() && action == GLFW_PRESS) {
        duplicateSelectedNodes();
    }
}

void ZNodeEditor::onScrollChange(double x, double y) {
    ZView::onScrollChange(x, y);

    // Scrolling with shift key is used for zooming charts
    if (!shiftKeyPressed()) {
        float scaleDelta = 1.0 + (y / 5.0);
        vec2 originalScale = mNodeContainer->getRelativeScale();
        vec2 newScale = max(vec2(0.3), min(vec2(1.0), originalScale * vec2(scaleDelta)));

        vec2 initialPos = mNodeContainer->getInnerTranslation();
        vec2 origin = vec2(getWidth() / 2, getHeight() / 2);

        vec2 scaled = ((initialPos - origin) * newScale) + origin;
        vec2 scaledZero = (initialPos * newScale);

        vec2 offset = scaled - scaledZero;

        mNodeContainer->setScale(newScale);
        mLineContainer->setScale(newScale);

        vec2 delta = (offset) / newScale;

        int margin = (int) ((float) NODE_CONTAINER_OFFSET / newScale.y);
        mNodeContainer->setYOffset(margin);
        mNodeContainer->setInnerTranslation(delta);
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
    mBoxSelect->setOffset(getMouse() / mNodeContainer->getScale());
    mBoxSelect->setMaxWidth(0);
    mBoxSelect->setMaxHeight(0);
}

void ZNodeEditor::updateBoxSelect() {
    vec2 p1 = mBoxSelect->getOffset();
    vec2 p2 = p1 + mBoxSelect->getSize();

    vec2 min1 = vec2(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
    vec2 max1 = vec2(std::max(p1.x, p2.x), std::max(p1.y, p2.y));

    for (ZNodeView* node : mNodeViews) {
        vec2 p3 = node->getOffset();
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

    if (mGrab || anyMouseDown()) {
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
        mCursorView->onWindowChange(getWidth(), getHeight());
        mCursorView->getParentView()->invalidate();
        invalidate();
    } else if (mBoxMode == BOX_SELECT_2) {

        vec2 mouse = getMouse() / mNodeContainer->getScale();
        int mouseX = (int) mouse.x;
        int mouseY = (int) mouse.y;
        mBoxSelect->setMaxWidth(mouseX - (int) mBoxSelect->getOffsetX());
        mBoxSelect->setMaxHeight(mouseY - (int) mBoxSelect->getOffsetY());
    }
}

template <typename T>
void ZNodeEditor::remove(std::vector<T>& vec, size_t pos){
    auto it = vec.begin();
    std::advance(it, pos);
    vec.erase(it);
}

bool ZNodeEditor::onMouseEvent(int button, int action, int mods, int sx, int sy) {
    bool mouseEvent = ZView::onMouseEvent(button, action, mods, sx, sy);

    // Quick connect nodes
    int nodeIndex = getMouseOverNode();
    if (!wasMouseDrag() && (button == GLFW_MOUSE_BUTTON_2 || button == GLFW_MOUSE_BUTTON_3) && action == GLFW_RELEASE && nodeIndex != -1) {
        if (mSecondLastSelected != nullptr) {
            quickConnectNodes(mSecondLastSelected, mNodeViews.at(nodeIndex));

        }
    }

    return mouseEvent;

}
