//
// Created by lukas on 8/4/20.
//

#include <ui/zlineview.h>
#include <ui/zlabel.h>
#include <ui/zbutton.h>
#include "ui/znodeview.h"

ZNodeView::ZNodeView(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight, parent) {
    setBackgroundColor(vec4(0.005130, 0.013321, 0.025381, 1.000000));

    ZLineView *lineView = new ZLineView(vec2(20, 20), vec2(200, 200), this);

    // Button example
    auto* addNodeBtn = new ZButton("Add node", parent);
    addNodeBtn->setOnClick([this](ZView* btn){
        addNode();
    });

}


void ZNodeView::addNode() {

    auto* node = new ZView(100, 150, this);
    node->setBackgroundColor(vec4(0.95, 0.95, 0.95, 1.0));
    node->setOffset(10, 10);

    ZLabel* nodeLabel = new ZLabel("Node", node);

    getParentView()->invalidate();
    mNodeViews.push_back(node);

}

void ZNodeView::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state) {
    ZView::onMouseDrag(absolute, start, delta, state);
    cout << delta.x << " " << delta.y << endl;
    if (state == mouseDown) {
        int i = 0;
        for(ZView* node : mNodeViews) {
            if (isMouseInBounds(node)) {
               mDragNode = i;
               mInitialOffset = vec2(node->getOffsetX(), node->getOffsetY());
               break;
            }

            i++;
        }
    } else if (state == mouseDrag && !mNodeViews.empty()) {
        mNodeViews.at(mDragNode)->setOffset((int) mInitialOffset.x + delta.x, (int) mInitialOffset.y + delta.y);
        cout << delta.x << " " << delta.y << endl;
        invalidate();
    } else {

    }



}
