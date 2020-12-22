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
    mBoxSelect->setDrawWire(WireType::outline);
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

    // Magnitude picker work
    mMagnitudePicker = new ZMagnitudePicker(this);
    mMagnitudePicker->setVisibility(false);

    thread evalThread = thread(ZNodeEditor::startEvaluation, this);
    evalThread.detach();

}

void ZNodeEditor::onLayoutFinished() {
    ZView::onLayoutFinished();
    addTestNodes();
}

void ZNodeEditor::addTestNodes() {
    ZNodeView *x = addNode(ZNodeView::X);
    ZNodeView *c = addNode(ZNodeView::CONSTANT);
    ZNodeView *s = addNode(ZNodeView::SIN);
    ZNodeView *co = addNode(ZNodeView::COS);
    ZNodeView *chart0 = addNode(ZNodeView::CHART_2D);

    connectNodes(0, 0, c, s);
    connectNodes(0, 0, x, s);

    connectNodes(0, 0, x, co);

    connectNodes(0, 0, s, chart0);
    connectNodes(0, 1, co, chart0);

    ZNodeView *lastChart = chart0;
    for (int i = 0; i < 7; i++) {
        ZNodeView *chart1 = addNode(ZNodeView::CHART_2D);
        connectNodes(0, 0, lastChart, chart1);
        connectNodes(1, 1, lastChart, chart1);
        lastChart = chart1;
    }

    updateLines();
}

/**
 * Starts the evaluation background thread
 */
void ZNodeEditor::startEvaluation(ZNodeEditor* editor) {

    // Todo: end loop when window closes
    bool shouldRun = true;

    while(shouldRun) {
        while (!editor->mEvalQueue.empty()) {
            ZNodeView *node = editor->mEvalQueue.front();
            node->updateChart();
            editor->mEvalQueue.pop();
            editor->mEvalSet.erase(node);
            glfwPostEmptyEvent();
        }

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

        if (mEvalSet.count(node) == 0) {
            mEvalSet.insert(node);
            mEvalQueue.push(node);
        }
        std::unique_lock<std::mutex> lck(mEvalMutex);
        mEvalConVar.notify_one();
    });

    node->setOnValueSelect([this](ZLabel* label, ZNodeView* nodeView){
        vec2 mouse = getMouse();

        // This logic shows the popup slider window
        float difference = (mMagnitudePicker->getWidth()) / 2.0;

        double xpos = std::min(std::max(0.0, (double) (mouse.x - difference)),
                               (double) getWidth() - mMagnitudePicker->getMaxWidth());
        double ypos;
        if (mouse.y - mMagnitudePicker->getMaxHeight() > 0) {
            ypos = mouse.y - mMagnitudePicker->getMaxHeight();
        } else {
            ypos = mouse.y + label->getHeight();
        }

        mMagnitudePicker->setOffset(vec2(xpos,ypos));
        mMagnitudePicker->setVisibility(true);
        mMagnitudePicker->onWindowChange(getWindowWidth(), getWindowHeight());
        mMagnitudePicker->setValueChangedListener([nodeView](float value){
            nodeView->setConstantValue({value});
        });
    });

    node->invalidateSingleNode();
    return node;
}

void ZNodeEditor::selectNode(ZNodeView* node) {
    if (mSelectedNodes.count(node) == 0 && !shiftKeyPressed()) {
        deselectAllNodes();
    }
    node->setOutlineColor(yellow);
    node->setLineWidth(3.0);
    mSelectedNodes.insert(node);
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
        int outputIndex = 0;
        vector<ZView*> outSockets = node->getSocketsOut();
        for (const vector<pair<ZNodeView*, int>>& nextNode : node->mOutputIndices) {
            if (!nextNode.empty()) {

                for (pair<ZNodeView*, int> inputIndex : node->mOutputIndices.at(outputIndex)) {
                    ZLineView* line = getLine(lineIndex++);
                    line->setPoints(outSockets.at(outputIndex)->getCenter(),
                                    inputIndex.first->getSocketsIn().at(inputIndex.second)->getCenter());
                    line->setVisibility(true);
                }
            }
            outputIndex++;
        }
    }
    getParentView()->invalidate();
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

void ZNodeEditor::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state) {
    ZView::onMouseDrag(absolute, start, delta, state);

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
    } else if (state == mouseDrag) {
        if (!mMagnitudePicker->getVisibility()) {
            onMouseMove(absolute, delta);
        }
    } else if (state == mouseUp) {
        onMouseUp();
    }
    updateLines();
}

void ZNodeEditor::onMouseDown() {
    int i = 0;

    if (mBoxMode == BOX_SELECT) {
        enterBoxSelect2nd();
    }

    bool mouseOverNode = false;
    for (ZNodeView *node : mNodeViews) {
        if (isMouseInBounds(node)) {
            mouseOverNode = true;
            mDragNode = i;
            selectNode(node);
            int j = 0;
            for (ZView *socket : node->getSocketsIn()) {
                if (isMouseInBounds(socket) && socket->getVisibility()) {

                    // If nothing connected show input line
                    if (node->mInputIndices.at(j).empty()) {
                        mDragType = SOCKET_DRAG_IN;
                        mInitialOffset = socket->getCenter();
                        mDragSocket = j;
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
                j++;
            }

            j = 0;
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
                mInitialOffset = vec2(node->getOffsetX(), node->getOffsetY());
                mInitialSize = vec2(node->getWidth(), node->getHeight());
            }
            break;
        }

        node->resetInitialPosition();
        i++;
    }

    if (!mouseOverNode) {
        deselectAllNodes();
    }

    updateLines();

}

void ZNodeEditor::onMouseMove(const vec2 &absolute, const vec2 &delta) {
    if (!mNodeViews.empty() && mDragNode != NO_SELECTION) {
        if (isSocketDrag()) {
            mTmpLine->setVisibility(true);
            mTmpLine->setPoints(absolute, mInitialOffset);
        } else {

            if (mouseIsDown()) {
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

            mAddNodePosition = vec2(DEFAULT_NODE_X, DEFAULT_NODE_Y);
        }
    }

    if (middleMouseIsDown() && !shiftKeyPressed()) {
        for (ZNodeView* node : mNodeViews) {
            node->setOffset(
                    (int) node->getInitialPosition().x + delta.x,
                    (int)  node->getInitialPosition().y + delta.y);
            node->onWindowChange(getWidth(), getHeight());
        }

        mAddNodePosition = vec2(DEFAULT_NODE_X, DEFAULT_NODE_Y);
    }

    getParentView()->getParentView()->getParentView()->invalidate();
}

void ZNodeEditor::onMouseUp() {
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
    vec2 mouseDelta = abs(mMouseDragDelta);
    if (mouseDelta.x < 3 && mouseDelta.y < 3 && !shiftKeyPressed() && nodeIndex != -1) {
        deselectAllNodes();
        selectNode(mNodeViews.at(nodeIndex));
    }

    mDragNode = NO_SELECTION;
    mDragType = NO_SELECTION;
    mDragSocket = NO_SELECTION;
    mTmpLine->setVisibility(false);

    if (mBoxMode == BOX_SELECT_2) {
        updateBoxSelect();
    }

    mMouseDragDelta = vec2(0);
    exitBoxSelectMode();
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
        if (isMouseInBounds(node)) {
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

void ZNodeEditor::updateBoxSelect() const { cout << "handle box select" << endl; }

void ZNodeEditor::onCursorPosChange(double x, double y) {
    ZView::onCursorPosChange(x, y);
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
