#include <utility>

//
// Created by lukas on 10/4/20.
//

#include <ui/zlabel.h>
#include <ui/zbutton.h>
#include <ui/zcheckbox.h>
#include <utils/zsettingsstore.h>
#include "ui/nodeview.h"

ZNodeView::ZNodeView(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight, parent) {

    mNameLabel = new ZLabel("Node", this);
    mNameLabel->setTextColor(grey);
    setBackgroundColor(vec4(0.95, 0.95, 0.95, 1.0));

    mOutputLabel = new ZLabel("1.0", this);
    mOutputLabel->setOffset(vec2(30, 0));
    mOutputLabel->setMaxWidth(80);

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
    evaluateBtn->setMaxWidth(15);
    evaluateBtn->setMaxHeight(15);
    evaluateBtn->setOnClick([this](){
        // todo: change input to something reasonable. Maybe zero, maybe pull from somewhere
        evaluate({0.0});
    });

    parent->invalidate();

    mChart = new ZChart(fillParent, fillParent, this);
    mChart->setMargin(vec4(20));
    mChart->setBackgroundColor(grey);
    mChart->setOffset(vec2(0,10));
    mChart->addLine({0,1,2,3,4,0,1,2,3,4});
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

void ZNodeView::onMouseEvent(int button, int action, int mods, int sx, int sy) {
    ZView::onMouseEvent(button, action, mods, sx, sy);

    if (action == GLFW_PRESS && isMouseInBounds(mOutputLabel)) {
        if (mListener != nullptr) {
            mListener(mOutputLabel, this);
            cout << "node label clicked" << endl;
        }

    }
}

void ZNodeView::setOnValueSelect(function<void(ZLabel *sender, ZNodeView *node)> onValueSelect) {
    mListener = std::move(onValueSelect);
}

void ZNodeView::setConstantValue(vector<float> value) {
    if (value.empty()) {
        mOutputLabel->setText("Bad input");
    } else {
        mOutputLabel->setText(to_string(value.at(0)));
        mConstantValue = value;
    }
}

vector<float> ZNodeView::evaluate(vector<float> x) {
    ivec2 size = getSocketCount();
    vector<float> output;
    if (x.size() < size.x) {
        //  mOutputLabel->setText(to_string(size.x) + " inputs needed, got " + to_string(x.size()));
        //  return vector<float>();
    } else {
        output = compute(x, mType);
    }

    if (size.x > 0) {
        vector<float> summedInputs = vector<float>((int) size.x);

        // Loop over input sockets
        for (int i = 0; i < size.x; i++) {
            const vector<pair<ZNodeView *, int>> &inputs = mInputIndices.at(i);

            // If inputs are connected evaluate recursively, otherwise use the specified input.
            if (!inputs.empty()) {
                // Summing all inputs is useful for dot products.
                float sum = 0;

                // Loop over all inputs on a single socket
                for (pair<ZNodeView *, int> input : inputs) {

                    // It's possible a previous node on the stack has too few inputs.
                    // When that happens display an error message.
                    vector<float> recurOutput = input.first->evaluate(x);
                    if (recurOutput.empty()) {
                        mOutputLabel->setText("Bad input");
                        setBackgroundColor(red);
                        mOutputLabel->setBackgroundColor(red);
                        mOutputLabel->setTextColor(white);
                        return vector<float>();
                    } else {
                        sum += input.first->evaluate(x).at(input.second);
                    }
                }
                summedInputs.at(i) = sum;
            } else {

                // Check that the passed input vector dimension matches
                // the number of input sockets on the node. If not
                // display an error message.
                if (x.size() <= size.x) {
                    mOutputLabel->setText(to_string(size.x) + " inputs needed, got " + to_string(x.size()));
                    setBackgroundColor(red);
                    mOutputLabel->setBackgroundColor(red);
                    mOutputLabel->setTextColor(white);
                    return vector<float>();
                } else {
                    summedInputs.at(i) = x.at(i);
                }
            }
        }
        output = compute(summedInputs, mType);
    }

    mOutputLabel->setText(to_string(output.at(0)));
    setBackgroundColor(white);
    mOutputLabel->setTextColor(black);
    mOutputLabel->setBackgroundColor(white);
    return output;

}
