//
// Created by lukas on 10/4/20.
//

#include <ui/zlabel.h>
#include "ui/nodeview.h"

ZNodeView::ZNodeView(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight, parent) {

    ZLabel* nodeLabel = new ZLabel("Node", this);

    setBackgroundColor(vec4(0.95, 0.95, 0.95, 1.0));
    setOffset(10, 10);

    // Add input sockets
    for (int i = 0; i < 3; i++) {
        float yOffset = 50;
        float margin = 10;

        auto* socket = new ZView(SOCKET_SIZE, SOCKET_SIZE, this);
        socket->setBackgroundColor(vec4(1, 0.611956, 0.052950, 1));
        socket->setOffset(0, yOffset + i * (SOCKET_SIZE + margin));
        mSocketsIn.push_back(socket);
    }

    // Add output sockets
    for (int i = 0; i < 3; i++) {
        float yOffset = 50;
        float margin = 10;

        auto* socket = new ZView(SOCKET_SIZE, SOCKET_SIZE, this);
        socket->setBackgroundColor(vec4(1, 0.611956, 0.052950, 1));
        socket->setOffset(maxWidth - SOCKET_SIZE, yOffset + i * (SOCKET_SIZE + margin));
        mSocketsOut.push_back(socket);
    }

    parent->invalidate();
}

void ZNodeView::getClostestSocket() {

}

vector<ZView *> ZNodeView::getSocketsIn() {
    return mSocketsIn;
}

vector<ZView *> ZNodeView::getSocketsOut() {
    return mSocketsOut;
}


