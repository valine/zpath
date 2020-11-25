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

void ZNodeEditor::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state) {
    ZView::onMouseDrag(absolute, start, delta, state);
    if (state == mouseDown) {
        int i = 0;
        for(ZNodeView* node : mNodeViews) {
            if (isMouseInBounds(node)) {
                for (ZView* inSocket : node->getSocketsIn()) {
                    socketMouseDown(inSocket);
                }

                for (ZView* inSocket : node->getSocketsOut()) {
                    socketMouseDown(inSocket);
                }


                if (mDragNode != SOCKET_DRAG) {
                    mDragNode = i;
                    mInitialOffset = vec2(node->getOffsetX(), node->getOffsetY());
                }
               break;
            }

            i++;
        }
    } else if (state == mouseDrag && !mNodeViews.empty() && mDragNode != NO_SELECTION) {

        if (mDragNode == SOCKET_DRAG) {
            mTmpLine->setVisibility(true);
            mTmpLine->setPoints(absolute, mInitialOffset);
        } else {
            mNodeViews.at(mDragNode)->setOffset((int) mInitialOffset.x + delta.x, (int) mInitialOffset.y + delta.y);
            mNodeViews.at(mDragNode)->onWindowChange(getWidth(), getHeight());
        }

        getParentView()->invalidate();
    } else if (state == mouseUp) {
        mDragNode = NO_SELECTION;
        mTmpLine->setVisibility(false);
    }



}

void ZNodeEditor::socketMouseDown(ZView *inSocket) {
    if (isMouseInBounds(inSocket)) {
        inSocket->setBackgroundColor(vec4(1, 0, 0, 1));
        mDragNode = SOCKET_DRAG;
        mInitialOffset = inSocket->getCenter();
    } else {
        inSocket->setBackgroundColor(vec4(0, 0, 0, 1));
    }
}
