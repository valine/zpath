#include <utility>

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

    setOutlineType(WireType::outline);
    setLineWidth(2.0);


    float yOffset = 3;
    float margin = 10;
    // Add input sockets

    auto socketType = getSocketType();

    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        auto* socket = new ZView(SOCKET_SIZE, SOCKET_SIZE, this);
        socket->setOffset(0, yOffset + i * (SOCKET_SIZE + margin));
        socket->setClickable(false);
        mSocketsIn.push_back(socket);
    }

    // Add output sockets
    for (int i = 0; i < MAX_OUTPUT_COUNT; i++) {
        auto* socket = new ZView(SOCKET_SIZE, SOCKET_SIZE, this);
        socket->setGravity(Gravity::topRight);
        socket->setOffset(0, yOffset + i * (SOCKET_SIZE + margin));
        socket->setClickable(false);
        mSocketsOut.push_back(socket);
    }

    initializeEdges();


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

        if (mChart->getInputCount() == 1) {
            if (mPointCache.empty()) {
                return vector<float>(2, 0);
            } else if (mPointCache.size() - 1 < x.at(0) || mPointCache.at(x.at(0)).empty()) {
                return vector<float>(2, 0);
            }
            return vector<float>(2, mPointCache.at(x.at(0)).at(0));
        } else {
            if (mPointCache.empty()) {
                return vector<float>(2, 0);
            }
            vector<float> point;
            point.push_back(mPointCache.at(x.at(0)).at(0));
            point.push_back(mPointCache.at(x.at(0)).at(1));


            return point;
        }
    });

    mChart->setInvalidateListener([this](){
        invalidateNodeRecursive();
    });
}

void ZNodeView::initializeEdges() {
    mInputIndices = vector<vector<pair<ZNodeView *, int>>>(MAX_INPUT_COUNT, vector<pair<ZNodeView *, int>>());
    mOutputIndices = vector<vector<pair<ZNodeView *, int>>>(MAX_OUTPUT_COUNT, vector<pair<ZNodeView *, int>>());
}

void ZNodeView::updateChart() {
    // This is usually run from background thread
    if (mInvalid) {
        if (mChart->getInputCount() == 1) {
            updateChart1D();
        } else {
            updateChart2D();
        }
    }
    clearInvalidateNode();
    mChart->requestLineUpdate();
   // mChart->invalidate();
}

void ZNodeView::updateChart2D() {
    // Data doesn't necessarily go off screen after this number of points. Finding this optimal resolution is
    // potentially complicated.
    int chartRes = mChart->getResolution();
    float increment = 0.1;

    mChart->computeChartBounds();
    vec2 xBounds = mChart->getXBounds();
    vec2 yBounds = mChart->getYBounds();

    vector<vector<float>> points;
    for (int i = 0; i < chartRes; i++) {
        float input = (float) i * increment;

        vector<float> fx = evaluate(vector<float>(MAX_INPUT_COUNT, input));
        if (fx.empty()) {
            mChart->setVisibility(false);
            return;
        }

        float ySpan = (yBounds.y - yBounds.x);
        float yFactor = (((fx.at(0)) - yBounds.x) / ySpan);

        float xSpan = (xBounds.y - xBounds.x);
        float xFactor = (((fx.at(1)) - xBounds.x) / xSpan);

        // xFactor and yFactor are the point coordinates in screen space
        points.push_back({xFactor, yFactor});
    }

    mPointCache = points;
}

void ZNodeView::updateChart1D() {
    int chartRes = mChart->getResolution();

    mChart->computeChartBounds();
    vec2 xBounds = mChart->getXBounds();
    vec2 yBounds = mChart->getYBounds();

    vector<vector<float>> points;
    for (int i = 0; i < mChart->getResolution(); i++) {
        float factor = (float) i / (float) chartRes;
        float x = mix(xBounds.x, xBounds.y, factor);
        vector<float> fx = evaluate(vector<float>(MAX_INPUT_COUNT, x));
        if (fx.empty()) {
            mChart->setVisibility(false);
            return;
        }

        float ySpan = (yBounds.y - yBounds.x);

        float yFactor = (((fx.at(0)) - yBounds.x) / ySpan);
        points.push_back({yFactor});

    }

    mPointCache = points;
}

void ZNodeView::setType(ZNodeView::Type type) {
    mType = type;
    ivec2 socketCount = getSocketCount();

    auto socketType = getSocketType();
    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        if (i >= socketCount.x) {
            mSocketsIn.at(i)->setVisibility(false);
        } else {
            if (i < socketCount.x) {
                if (socketType.first.at(i) == VAR) {
                    mSocketsIn.at(i)->setBackgroundColor(mVariableColor);
                } else {
                    mSocketsIn.at(i)->setBackgroundColor(mConstantColor);
                }
            }
        }
    }
    vec4 darkenVec = vec4(0.5,0.4,0.4,0.0);
    for (int i = 0; i < MAX_OUTPUT_COUNT; i++) {
        if (i >= socketCount.y) {
            mSocketsOut.at(i)->setVisibility(false);
        } else {
            if (i < socketCount.y) {
                if (socketType.second.at(i) == VAR) {
                    mSocketsOut.at(i)->setBackgroundColor(mVariableColor - darkenVec);
                } else {
                    mSocketsOut.at(i)->setBackgroundColor(mConstantColor - darkenVec);
                }
            }
        }
    }


    int varCount = 0;
    for (auto inputType : socketType.second) {
        if (inputType == VAR) {
            varCount++;
        }
    }
    mChart->setInputCount(std::max(varCount, 1));

    mNameLabel->setText(getName(mType));
    mOutputLabel->setVisibility(isOutputLabelVisible(mType));
    setBackgroundColor(getNodeColor(mType));

    if (isOutputLabelVisible(mType)) {
        mOutputLabel->setText(to_string(mConstantValueOutput.at(0)));
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

    int i = 0;
    for (float cValue : getDefaultInput(mType)) {
        mConstantValueInput.at(i) = cValue;
        i++;
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

    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1) {
        if (isMouseInBounds(mOutputLabel) &&
            mOutputLabel->getVisibility()) {
            if (mShowMagnitudeView != nullptr) {
                mShowMagnitudeView(mOutputLabel, this, 0, false);
            }
        } else {
            int socketIndex = 0;
            for (ZView* inputSocket : mSocketsIn) {
                if (isMouseInBounds(inputSocket) && !shiftKeyPressed() &&
                        inputSocket->getVisibility() && getSocketType().first.at(socketIndex) == CON) {
                    mShowMagnitudeView(mOutputLabel, this, socketIndex, true);
                }
                socketIndex++;
            }

        }
    }

    if (isMouseInBounds(mChart) && ((middleMouseIsDown() && shiftKeyPressed()) || (rightMouseIsDown() && shiftKeyPressed()))) {
        return true;
    }

    return ZView::onMouseEvent(button, action, mods, sx, sy);
}

void ZNodeView::setOnValueSelect(function<void(ZView *sender, ZNodeView *node, int index, bool isInput)> onValueSelect) {
    mShowMagnitudeView = std::move(onValueSelect);
}

void ZNodeView::setConstantValue(int index, float value) {
    if (index >= mConstantValueOutput.size()) {
        mOutputLabel->setText("Bad input");
    } else {
        mConstantValueOutput.at(index) = value;
        mOutputLabel->setText(to_string(mConstantValueOutput.at(0)));
        invalidateNodeRecursive();
    }
}

void ZNodeView::setConstantValueInput(int index, float value) {
    if (index >= mConstantValueOutput.size()) {
        mOutputLabel->setText("Bad input");
    } else {
        mConstantValueInput.at(index) = value;
        invalidateNodeRecursive();
    }
}


vector<float> ZNodeView::evaluate(vector<float> x) {
    return evaluate(std::move(x), nullptr);
}

vector<float> ZNodeView::evaluate(vector<float> x, ZNodeView* root) {
    ivec2 size = getSocketCount();
    vector<float> output;
    if (x.size() >= size.x) {
        output = compute(x, mType);
    }

    if (size.x > 0) {
        vector<float> summedInputs = vector<float>((int) MAX_INPUT_COUNT, 0);

        // Loop over input sockets
        for (int i = 0; i < size.x; i++) {
            if (mInputIndices.empty()) {
                return output;
            }

            const vector<pair<ZNodeView *, int>> &inputs = mInputIndices.at(i);

            // If inputs are connected evaluate recursively, otherwise use the specified input.
            if (!inputs.empty()) {
                // Summing all inputs is useful for dot products.
                float sum = 0;
                // Loop over all inputs on a single socket
                for (pair<ZNodeView *, int> input : inputs) {

                    // Detect case where the output node is connected to the input node
                    if (root == this) {
                        mOutputLabel->setText("Infinite Loop");
                        return vector<float>((int) MAX_INPUT_COUNT, 0);
                    }

                    vector<float> recurOutput;
                    if (root == nullptr) {
                        recurOutput = input.first->evaluate(x, this);
                    } else {
                        recurOutput = input.first->evaluate(x, root);
                    }

                    // It's possible a previous node on the stack has too few inputs.
                    // When that happens display an error message.
                    if (recurOutput.empty()) {
                        mOutputLabel->setText("Bad input");
                        //mOutputLabel->setBackgroundColor(red);

                        mOutputLabel->setTextColor(red);
                        return vector<float>();
                    } else {
                        sum += recurOutput.at(input.second);
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

                    // Use the default input when nothing is connected to a constant socket
                    if (getSocketType().first.at(i) == VAR) {
                        summedInputs.at(i) = x.at(i);
                    } else {
                        summedInputs.at(i) = mConstantValueInput.at(i);
                    }

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


    if (mChart->getInputCount() == 1) {
        int newRes = (int) (getWidth() / 4.0);
        if (abs(newRes - mChart->getResolution()) > CHART_RES_THRESHOLD) {
            mChart->setResolution(newRes);
            invalidateSingleNode();
        }
    } else {
        // Data doesn't necessarily go off screen after this number of points. Finding this optimal resolution is
        // potentially complicated.
         mChart->setResolution(100);
        //invalidateSingleNode();
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
    //setBackgroundColor(blue); // Turn this to another color to debug invalidation logic.
    mInvalid = true;

    if (mInvalidateListener != nullptr) {
        mInvalidateListener(this);
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