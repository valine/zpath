//
// Created by lukas on 8/4/20.
//

#include <ui/zlineview.h>
#include <ui/zlabel.h>
#include <ui/zbutton.h>
#include <ui/nodeview.h>
#include "ui/znodeeditor.h"

ZNodeEditor::ZNodeEditor(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight, parent) {
    setBackgroundColor(vec4(0.005130, 0.013321, 0.025381, 1.000000));

    mTmpLine = new ZLineView(vec2(20, 20), vec2(200, 200), this);
    mTmpLine->setVisibility(false);

    ZLineView* line = new ZLineView(vec2(0), vec2(0), this);
    mLineBucket.push_back(line);
    // Button example
    auto* addNodeBtn = new ZButton("Add node", parent);
    addNodeBtn->setOnClick([this](ZView* btn){
        addNode();
    });



}


void ZNodeEditor::addNode() {
    auto* node = new ZNodeView(100, 150, this);
    mNodeViews.push_back(node);
}

void ZNodeEditor::updateLines() {
    for (ZLineView* lineView : mLineBucket) {
        lineView->setVisibility(false);
    }

    int lineIndex = 0;
    for (ZNodeView* node : mNodeViews) {
        int outputIndex = 0;
        for (ZNodeView* nextNode : node->mOutputs) {
            if (nextNode != nullptr) {
                int inputIndex = node->mOutputIndices.at(outputIndex);
                ZLineView* line = getLine(lineIndex++);
                line->setPoints(node->getSocketsOut().at(outputIndex)->getCenter(),
                        nextNode->getSocketsIn().at(inputIndex)->getCenter());
                line->setVisibility(true);
            }
            outputIndex++;
        }
    }

    getParentView()->invalidate();
}

ZLineView* ZNodeEditor::getLine(int index) {
    if (mLineBucket.size() >= index + 2) {
        return mLineBucket.at(index);
    } else {
        ZLineView* line = new ZLineView(vec2(0), vec2(0), this);
        mLineBucket.push_back(line);
        return mLineBucket.at(index);
    }
}

void ZNodeEditor::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state) {
    ZView::onMouseDrag(absolute, start, delta, state);
    if (state == mouseDown) {
        int i = 0;
        for(ZNodeView* node : mNodeViews) {
            if (isMouseInBounds(node)) {
                mDragNode = i;
                int j = 0;
                for (ZView* socket : node->getSocketsIn()) {
                    if (isMouseInBounds(socket)) {
                        mDragType = SOCKET_DRAG_IN;
                        mInitialOffset = socket->getCenter();
                        mDragSocket = j;
                    }
                    j++;
                }

                j = 0;
                for (ZView* socket : node->getSocketsOut()) {
                    if (isMouseInBounds(socket)) {
                        mDragType = SOCKET_DRAG_OUT;
                        mInitialOffset = socket->getCenter();
                        mDragSocket = j;
                    }
                    j++;
                }

                if (!isSocketDrag()) {
                    mDragType = NODE_DRAG;
                    mInitialOffset = vec2(node->getOffsetX(), node->getOffsetY());
                }
               break;
            }

            i++;
        }
    } else if (state == mouseDrag && !mNodeViews.empty() && mDragNode != NO_SELECTION) {

        if (isSocketDrag()) {
            mTmpLine->setVisibility(true);
            mTmpLine->setPoints(absolute, mInitialOffset);
        } else {
            mNodeViews.at(mDragNode)->setOffset(
                    (int) mInitialOffset.x + delta.x,
                    (int) mInitialOffset.y + delta.y);
            mNodeViews.at(mDragNode)->onWindowChange(getWidth(), getHeight());

        }
    } else if (state == mouseUp) {

        int nodeIndex = getMouseOverNode();

        if (isSocketDrag() && nodeIndex != NO_SELECTION) {
            int socketOutIndex = getMouseOverOutSocket(mNodeViews.at(nodeIndex));
            int socketInIndex = getMouseOverInSocket(mNodeViews.at(nodeIndex));

            ZNodeView *activeNode = mNodeViews.at(mDragNode);
            ZNodeView *targetNode = mNodeViews.at(nodeIndex);
            switch (mDragType) {
                case SOCKET_DRAG_IN:
                    if (socketOutIndex != NO_SELECTION) {
                        targetNode->mOutputs.at(socketOutIndex) = activeNode;
                        activeNode->mInputs.at(mDragSocket) = targetNode;

                        targetNode->mOutputIndices.at(socketOutIndex) = mDragSocket;
                        activeNode->mInputIndices.at(mDragSocket) = socketOutIndex;
                    }
                    break;
                case SOCKET_DRAG_OUT:
                    if (socketInIndex != NO_SELECTION) {
                        targetNode->mInputs.at(socketInIndex) = activeNode;
                        activeNode->mOutputs.at(mDragSocket) = targetNode;

                        targetNode->mInputIndices.at(socketInIndex) = mDragSocket;
                        activeNode->mOutputIndices.at(mDragSocket) = socketInIndex;
                    }
                    break;
                case NO_SELECTION:
                    break;
                default:
                    break;
            }


        }

        mDragNode = NO_SELECTION;
        mDragType = NO_SELECTION;
        mDragSocket = NO_SELECTION;
        mTmpLine->setVisibility(false);


    }

    updateLines();

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

