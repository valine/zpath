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
#include "ui/znodeeditor.h"

ZNodeEditor::ZNodeEditor(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight, parent) {
    setBackgroundColor(vec4(0.005130, 0.013321, 0.025381, 1.000000));

    mNodeContainer = new ZView(fillParent, fillParent, this);
    mLineContainer = new ZView(fillParent, fillParent, this);
    mNodeContainer->setYOffset(NODE_CONTAINER_OFFSET);
    
    mHeader = new ZView(fillParent, fillParent, this);

    mTmpLine = new ZLineView(vec2(20, 20), vec2(200, 200), mLineContainer);
    mTmpLine->setVisibility(false);

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

    ZDropDown* dropDown = new ZDropDown(100,500, allTypes, mHeader);
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

bool ZNodeEditor::needsEval() {
    return !mEvalSet.empty();
}


/**
 * Starts the evaluation background thread
 */
void ZNodeEditor::startEvaluation(ZNodeEditor* editor) {

    bool shouldRun = true;
    while(shouldRun) {
        while (!editor->mEvalQueue.empty()) {
            ZNodeView *node = editor->mEvalQueue.front();
            node->updateChart();
            editor->mEvalSet.erase(node);
            editor->mEvalQueue.pop();
            glfwPostEmptyEvent();
        }

        std::unique_lock<std::mutex> lck(editor->mEvalMutex);
        editor->mEvalConVar.wait(lck);
    }
}


void ZNodeEditor::addNode(ZNodeView::Type type) {
    mLastType = type;

    auto* node = new ZNodeView(NODE_WIDTH, NODE_HEIGHT, mNodeContainer);
    mNodeViews.push_back(node);

    node->setIndexTag(mNodeViews.size() - 1);

    vec2 scale = mNodeContainer->getScale();
    vec2 tranlsation = mNodeContainer->getInnerTranslation();

    node->setOffset(mAddNodePosition - tranlsation);
    node->setInitialPosition((mAddNodePosition - tranlsation) - getMouseDragDelta());
    
    if (mAddNodePosition.x + NODE_WIDTH > getWidth() / scale.x) {
        mAddNodePosition.x = DEFAULT_NODE_X;
        mAddNodePosition.y += NODE_HEIGHT + NODE_MARGIN;
    } else {
        mAddNodePosition.x += NODE_WIDTH + NODE_MARGIN;
    }

    node->setType(type);
    node->onWindowChange(getWidth(), getHeight());
    node->evaluate(vector<float>(MAX_INPUT_COUNT, 3.0));
    node->updateChart();

    node->setOnValueSelect([this](ZLabel* label, ZNodeView* nodeView){
        vec2 mouse = getMouse();
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

    node->setInvalidateListener([this](ZNodeView* node){
        std::unique_lock<std::mutex> lck(mEvalMutex);

        if (mEvalSet.count(node) == 0) {
            mEvalQueue.push(node);
            mEvalSet.insert(node);
        }

        mEvalConVar.notify_one();
    });
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
    for (ZNodeView *node : mNodeViews) {
        if (isMouseInBounds(node)) {
            mDragNode = i;
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

                       node->invalidateNode();
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

    updateLines();

}

void ZNodeEditor::onMouseMove(const vec2 &absolute, const vec2 &delta) {
    if (!mNodeViews.empty() && mDragNode != NO_SELECTION) {
        if (isSocketDrag()) {
            mTmpLine->setVisibility(true);
            mTmpLine->setPoints(absolute, mInitialOffset);
        } else {

            if (mouseIsDown()) {
                mNodeViews.at(mDragNode)->setOffset(
                        (int) mInitialOffset.x + delta.x,
                        (int) mInitialOffset.y + delta.y);


            } else if (rightMouseIsDown()) {
                mNodeViews.at(mDragNode)->setMaxWidth(
                        mInitialSize.x + delta.x);
                mNodeViews.at(mDragNode)->setMaxHeight(
                        mInitialSize.y + delta.y);

            }
            mNodeViews.at(mDragNode)->onWindowChange(getWidth(), getHeight());
            mNodeViews.at(mDragNode)->invalidate();
            mAddNodePosition = vec2(DEFAULT_NODE_X, DEFAULT_NODE_Y);
        }
    }

    if (middleMouseIsDown()) {
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
                    targetNode->mOutputIndices.at(socketOutIndex).push_back(pair<ZNodeView*, int>(activeNode, mDragSocket));
                    activeNode->mInputIndices.at(mDragSocket).push_back(pair<ZNodeView*, int>(targetNode, socketOutIndex));

                    activeNode->evaluate(vector<float>(MAX_INPUT_COUNT, 3.0));
                    targetNode->evaluate(vector<float>(MAX_INPUT_COUNT, 3.0));

                    activeNode->invalidateNode();
                }
                break;
            case SOCKET_DRAG_OUT:
                if (socketInIndex != NO_SELECTION) {
                    targetNode->mInputIndices.at(socketInIndex).push_back(pair<ZNodeView*, int>(activeNode, mDragSocket));
                    activeNode->mOutputIndices.at(mDragSocket).push_back(pair<ZNodeView*, int>(targetNode, socketInIndex));

                    activeNode->evaluate(vector<float>(MAX_INPUT_COUNT, 3.0));
                    targetNode->evaluate(vector<float>(MAX_INPUT_COUNT, 3.0));

                    targetNode->invalidateNode();
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

    mDragNode = NO_SELECTION;
    mDragType = NO_SELECTION;
    mDragSocket = NO_SELECTION;
    mTmpLine->setVisibility(false);
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

    if (key == GLFW_KEY_E && shiftKeyPressed()) {
        addNode(mLastType);
    }
}

void ZNodeEditor::onScrollChange(double x, double y) {
    ZView::onScrollChange(x, y);

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

template <typename T>
void ZNodeEditor::remove(std::vector<T>& vec, size_t pos){
    auto it = vec.begin();
    std::advance(it, pos);
    vec.erase(it);
}