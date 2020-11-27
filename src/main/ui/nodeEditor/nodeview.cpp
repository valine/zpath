//
// Created by lukas on 10/4/20.
//

#include <ui/zlabel.h>
#include <ui/zbutton.h>
#include <ui/zcheckbox.h>
#include "ui/nodeview.h"

ZNodeView::ZNodeView(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight, parent) {

    mNameLabel = new ZLabel("Node", this);
    setBackgroundColor(vec4(0.95, 0.95, 0.95, 1.0));

    mOutputLabel = new ZLabel("1.0", this);
    mOutputLabel->setOffset(vec2(30, 30));

    //evaluate->setOffset(vec2(0, 100));

    setDrawWire(WireType::outline);

    float yOffset = 30;
    float margin = 10;
    // Add input sockets
    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        auto* socket = new ZView(SOCKET_SIZE, SOCKET_SIZE, this);
        socket->setBackgroundColor(vec4(1, 0.611956, 0.052950, 1));
        socket->setOffset(0, yOffset + i * (SOCKET_SIZE + margin));
        socket->setClickable(false);
        mSocketsIn.push_back(socket);
    }

    // Add output sockets
    for (int i = 0; i < MAX_OUTPUT_COUNT; i++) {
        auto* socket = new ZView(SOCKET_SIZE, SOCKET_SIZE, this);
        socket->setBackgroundColor(vec4(0.429225, 0.213118, 0.021705, 1));
        socket->setGravity(Gravity::topRight);
        socket->setOffset(0, yOffset + i * (SOCKET_SIZE + margin));
        socket->setClickable(false);
        mSocketsOut.push_back(socket);
    }

    mInputs = vector<ZNodeView*>(MAX_INPUT_COUNT, nullptr);
    mOutputs = vector<ZNodeView*>(MAX_OUTPUT_COUNT, nullptr);

    mInputIndices = vector<vector<pair<ZNodeView*, int>>>(MAX_INPUT_COUNT, vector<pair<ZNodeView*, int>>());
    mOutputIndices = vector<vector<pair<ZNodeView*, int>>>(MAX_OUTPUT_COUNT,  vector<pair<ZNodeView*, int>>());

    ZButton* evaluateBtn = new ZButton("->", this);
    evaluateBtn->setGravity(Gravity::bottomRight);
    evaluateBtn->setMaxWidth(30);
    evaluateBtn->setMaxHeight(20);
    evaluateBtn->setOnClick([this](){
        cout << "evaluate" << endl;
        evaluate({3.14159});
    });

    parent->invalidate();

}

void ZNodeView::setType(ZNodeView::Type type) {
    mType = type;
    vec2 socketCount = getSocketCount();


    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        if (i >= socketCount.x) {
            mSocketsIn.at(i)->setVisibility(false);
        }
    }

    for (int i = 0; i < MAX_OUTPUT_COUNT; i++) {
        if (i >= socketCount.y) {
            mSocketsOut.at(i)->setVisibility(false);
        }
    }
    mNameLabel->setText(getName(mType));
}


vector<ZView *> ZNodeView::getSocketsIn() {
    return mSocketsIn;
}

vector<ZView *> ZNodeView::getSocketsOut() {
    return mSocketsOut;
}
