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
    mNameLabel->setTextColor(black);
    mNameLabel->setXOffset(30);
    setBackgroundColor(vec4(0.95, 0.95, 0.95, 1.0));

    mOutputLabel = new ZLabel("", this);
    mOutputLabel->setOffset(vec2(20, 0));
    mOutputLabel->setMaxWidth(40);
    mOutputLabel->setGravity(Gravity::topRight);

    setDrawWire(WireType::outline);

    float yOffset = 3;
    float margin = 10;
    // Add input sockets
    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        auto* socket = new ZView(SOCKET_SIZE, SOCKET_SIZE, this);
        socket->setBackgroundColor(mVariableColor);
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

    mInputIndices = vector<vector<pair<ZNodeView*, int>>>(MAX_INPUT_COUNT, vector<pair<ZNodeView*, int>>());
    mOutputIndices = vector<vector<pair<ZNodeView*, int>>>(MAX_OUTPUT_COUNT,  vector<pair<ZNodeView*, int>>());


    // Manually trigger evaluation. This was needed before the async eval was working.
    // May be useful for debugging in the future. If so set visibility to true.
    ZButton* evaluateBtn = new ZButton("->", this);
    evaluateBtn->setGravity(Gravity::bottomRight);
    evaluateBtn->setMaxWidth(15);
    evaluateBtn->setMaxHeight(15);
    evaluateBtn->setVisibility(false);
    evaluateBtn->setOnClick([this](){
        // todo: change input to something reasonable. Maybe zero, maybe pull from somewhere
        //evaluate({0.0});
        // updateChart();
    });

    parent->invalidate();

    mChart = new ZLineChart(fillParent, fillParent, this);
    mChart->setMargin(vec4(MIN_MARGIN, CHART_TOP_MARGIN, MIN_MARGIN, MIN_MARGIN));
    mChart->setBackgroundColor(grey);
    mChart->setOffset(vec2(0,10));
    mChart->setChartListener([this](vector<int> x, int lineIndex){
        // Can safely ignore line index for now

        if (mPointCache.empty()) {
            return vector<float>(mChart->getResolution(), 0);
        }
        vector<float> line;
        line.push_back(mPointCache.at(x.at(0)));
        return line;
    });
}


void ZNodeView::updateChart() {
    // This is usually run from background thread
    if (mInvalid) {
        int chartRes = mChart->getResolution();
        vec2 xBounds = mChart->getXBounds();

        vector<float> points;
        for (int i = 0; i < mChart->getResolution(); i++) {
            float factor = (float) i / (float) chartRes;
            float x = mix(xBounds.x, xBounds.y, factor);
            vector<float> fx = evaluate(vector<float>(MAX_INPUT_COUNT, x));
            if (fx.empty()) {
                mChart->setVisibility(false);
                return;
            }
            points.push_back(evaluate(vector<float>(MAX_INPUT_COUNT, x)).at(0));
        }

        mPointCache = points;
        clearInvalidateNode();
        mChart->requestLineUpdate();
        mChart->invalidate();
    }
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
    mOutputLabel->setVisibility(isOutputLabelVisible(mType));
    setBackgroundColor(getNodeColor(mType));

    if (isOutputLabelVisible(mType)) {
        mOutputLabel->setText(to_string(mConstantValue.at(0)));
        mOutputLabel->setBackgroundColor(getNodeColor(mType));
    }

    if (getSocketCount().x == 0) {
        mNameLabel->setXOffset(MIN_MARGIN);
    }

    if (getSocketCount().x > 1) {
        mChart->setMarginLeft(CHART_SIDE_MARGIN_WIDE);
    }

    if (getSocketCount().y > 1) {
        mChart->setMarginRight(CHART_SIDE_MARGIN_WIDE);
    }
}


vector<ZView *> ZNodeView::getSocketsIn() {
    return mSocketsIn;
}

vector<ZView *> ZNodeView::getSocketsOut() {
    return mSocketsOut;
}

bool ZNodeView::onMouseEvent(int button, int action, int mods, int sx, int sy) {
    ZView::onMouseEvent(button, action, mods, sx, sy);

    // Todo: logic needed to reject click events during graph translation


    if (action == GLFW_PRESS && isMouseInBounds(mOutputLabel)) {
        if (mListener != nullptr) {
            mListener(mOutputLabel, this);
        }
    }

    if (isMouseInBounds(mChart) && middleMouseIsDown()) {
        return true;
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
        invalidateNodeRecursive();
    }
}

vector<float> ZNodeView::evaluate(vector<float> x) {
    ivec2 size = getSocketCount();
    vector<float> output;
    if (x.size() >= size.x) {
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
                        //mOutputLabel->setBackgroundColor(red);
                        mOutputLabel->setTextColor(red);
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
                    //mOutputLabel->setBackgroundColor(red);
                    mOutputLabel->setTextColor(white);
                    return vector<float>();
                } else {
                    summedInputs.at(i) = x.at(i);
                }
            }
        }
        output = compute(summedInputs, mType);
    }

    //mOutputLabel->setText(to_string(output.at(0)));
//    mOutputLabel->setTextColor(black);
//    mOutputLabel->setBackgroundColor(white);
    return output;

}

void ZNodeView::onWindowChange(int windowWidth, int windowHeight) {
    ZView::onWindowChange(windowWidth, windowHeight);

    // Todo: update this logic for new line chart
    int newRes = (int) (getWidth() / 7.0);
    if (abs(newRes - mChart->getResolution()) > CHART_RES_THRESHOLD) {
        mChart->setResolution(newRes);
        invalidateSingleNode();
    } else {
        //clearInvalidateNode();
    }
}

void ZNodeView::clearInvalidateNode() {
    //setBackgroundColor(white);  // Turn this to another color to debug invalidation logic.
    mInvalid = false;
}

/**
 * Only invalidate this node, don't update children.
 */
void ZNodeView::invalidateSingleNode() {
    //setBackgroundColor(white); // Turn this to another color to debug invalidation logic.
    mInvalid = true;

    if (mInvalidateListener != nullptr) {
        mInvalidateListener(this);
    }  else {

        cout << "Node evaluation happening on main ui thread. "
                "Set the node invalidate listener if the UI is hanging." << endl;
        //updateChart(); // Todo: review if this udate chart is needed
        //mChart->requestLineUpdate();
    }
}

/**
 * Invalidate this node and all child nodes
 */
void ZNodeView::invalidateNodeRecursive() {
    ZView::invalidate();
    invalidateSingleNode();

    for (const vector<pair<ZNodeView*, int>>& outputSocket : mOutputIndices) {
        for (pair<ZNodeView*, int> child : outputSocket) {
            if (!child.first->isInvalid()) {
                child.first->invalidateNodeRecursive();
            }
        }
    }
}

bool ZNodeView::isInvalid() {
    return mInvalid;
}



void ZNodeView::setInvalidateListener(function<void(ZNodeView *node)> listener) {
    mInvalidateListener = std::move(listener);
}