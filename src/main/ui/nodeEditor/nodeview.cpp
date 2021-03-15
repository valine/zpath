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
#include <sstream>
#include <iomanip>
#include <mutex>

ZNodeView::ZNodeView(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight, parent) {
    mChart = new ZLineChart(fillParent, fillParent, this);

    mNameLabel = new ZLabel("Node", this);
    mNameLabel->setTextColor(black);
    mNameLabel->setXOffset(22);
    mNameLabel->setYOffset(1);
    setBackgroundColor(vec4(0.95, 0.95, 0.95, 1.0));

    mOutputLabel = new ZLabel("", this);
    mOutputLabel->setOffset(vec2(20, 0));
    mOutputLabel->setMaxWidth(40);
    mOutputLabel->setYOffset(1);
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
    parent->invalidate();

    mChart->setMargin(vec4(MIN_MARGIN, CHART_TOP_MARGIN, MIN_MARGIN, MIN_MARGIN));

    mChart->setOffset(vec2(0,10));
    mChart->setChartListener([this](vector<int> x, int lineIndex){
        // Can safely ignore line index for now

        if (getChartType(getType()) == LINE_1D || getChartType(getType()) == LINE_1D_2X) {
            if (mPointCache.empty()) {
                return vector<float>(1, 0);
            } else if (mPointCache.size() - 1 < x.at(0) || mPointCache.at(x.at(0)).empty()) {
                return vector<float>(1, 0);
            }
            return vector<float>(1, mPointCache.at(x.at(0)).at(lineIndex));
        } else if (getChartType(getType()) == LINE_2D) {
            if (mPointCache.empty() || mPointCache.size() <= x.at(0)) {
                return vector<float>(2, 0);
            }
            vector<float> point;

            point.push_back(mPointCache.at(x.at(0)).at(0));
            point.push_back(mPointCache.at(x.at(0)).at(1));

            return point;
        } else if (getChartType(getType()) == ChartType::IMAGE) {
            return mPointCache1D;
        }
    });

    mChart->setInvalidateListener([this](){
        invalidateNodeRecursive();
    });

    // Bound labels
    vec4 boundColor = vec4(0.4,0.4,0.4,1.0);
    int boundLabelWidth = 50;
    mXMinLabel = new ZLabel("xmin", mChart);
    mXMinLabel->setMaxWidth(boundLabelWidth);
    mXMinLabel->setGravity(bottomLeft);
    //mXMinLabel->setBackgroundColor(vec4(0));
    mXMinLabel->setTextColor(boundColor);

    mXMaxLabel = new ZLabel("xmax", mChart);
    mXMaxLabel->setMaxWidth(boundLabelWidth);
    mXMaxLabel->setGravity(bottomRight);
    mXMaxLabel->setMargin(vec4(0));
    //mXMaxLabel->setBackgroundColor(vec4(0));
    mXMaxLabel->setXOffset(-20);
    mXMaxLabel->setTextColor(boundColor);

    mYMinLabel = new ZLabel("ymin", mChart);
    mYMinLabel->setMaxWidth(boundLabelWidth);
    mYMinLabel->setGravity(bottomLeft);
    mYMinLabel->setYOffset(20);
    //mYMinLabel->setBackgroundColor(vec4(0));
    mYMinLabel->setTextColor(boundColor);

    mYMaxLabel = new ZLabel("ymax", mChart);
    mYMaxLabel->setMaxWidth(boundLabelWidth);
    mYMaxLabel->setGravity(topLeft);
    //mYMaxLabel->setBackgroundColor(vec4(0));
    mYMaxLabel->setTextColor(boundColor);

    updateLabelVisibility();
}

void ZNodeView::initializeEdges() {
    mInputIndices = vector<vector<pair<ZNodeView *, int>>>(MAX_INPUT_COUNT, vector<pair<ZNodeView *, int>>());
    mOutputIndices = vector<vector<pair<ZNodeView *, int>>>(MAX_OUTPUT_COUNT, vector<pair<ZNodeView *, int>>());
}

void ZNodeView::updateChart() {
    // This is run from background thread
    if (mInvalid) {
        clearInvalidateNode();
        mFftCache.clear();
        if (getChartType(getType()) == LINE_1D) {
            updateChart1D();
        } else if (getChartType(getType()) == LINE_2D) {
            updateChart2D();
        } else if (getChartType(getType()) == LINE_1D_2X) {
            updateChart1D2X();
        } else if (getChartType(getType()) == IMAGE) {
            mLaplaceCache.clear();
            updateChartHeatMap();
        }
    }

    mChart->invalidateData();
    vec2 xBound = mChart->getXBounds();
    vec2 yBound = mChart->getYBounds();

    std::stringstream xmin;
    xmin << std::fixed << std::setprecision(2) << xBound.x;
    std::stringstream xmax;
    xmax << std::fixed << std::setprecision(2) << xBound.y;

    std::stringstream ymin;
    ymin << std::fixed << std::setprecision(2) << yBound.x;
    std::stringstream ymax;
    ymax << std::fixed << std::setprecision(2) << yBound.y;
    mXMinLabel->setText(xmin.str());
    mXMaxLabel->setText(xmax.str());
    mYMinLabel->setText(ymin.str());
    mYMaxLabel->setText(ymax.str());
}

void ZNodeView::updateChartHeatMap() {

    // todo: update this when x and y resolution defined independently
    int xRes = std::min(mChart->getResolution(), mChart->getMaxResolution());
    int yRes = std::min(mChart->getResolution(), mChart->getMaxResolution());

    mChart->computeChartBounds();
    vec2 xBounds = mChart->getXBounds();
    vec2 yBounds = mChart->getYBounds();

    vector<float> points;

    for (int iy = 0; iy < yRes; iy++) {
        for (int ix = 0; ix < xRes; ix++) {
            float xFactor = (float) ix / (float) xRes;
            float yFactor = (float) iy / (float) yRes;

            float x = mix(xBounds.x, xBounds.y, xFactor);
            float y = mix(yBounds.x, yBounds.y, yFactor);
            vector<vector<float>> inVec = {vector<float>(MAX_INPUT_COUNT, x), vector<float>(MAX_INPUT_COUNT, y)};

            vector<vector<float>> z = evaluate(inVec);
            if (!z.empty()) {
                points.push_back(z.at(0).at(0));
            }
        }
    }

    mPointCache1D = points;
}

void ZNodeView::updateChart2D() {
    // Data doesn't necessarily go off screen after this number of points.
    // Finding this optimal resolution is potentially complicated.
    float increment = 0.1;

    mChart->computeChartBounds();
    vec2 xBounds = mChart->getXBounds();
    vec2 yBounds = mChart->getYBounds();
    evaluate(evaluate(vector<vector<float>>(2, vector<float>(MAX_INPUT_COUNT, 0))));
    vector<vector<float>> points;
    for (int i = 0; i < mChart->getResolution(); i++) {
        float input = (float) i * increment;

        vector<vector<float>> fx = evaluate(vector<vector<float>>(2, vector<float>(MAX_INPUT_COUNT, input)));
        if (fx.empty()) {
            mChart->setVisibility(false);
            return;
        }

        float ySpan = (yBounds.y - yBounds.x);
        float yFactor = (((fx.at(0).at(0)) - yBounds.x) / ySpan);

        float xSpan = (xBounds.y - xBounds.x);
        float xFactor = (((fx.at(0).at(1)) - xBounds.x) / xSpan);

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
        vector<vector<float>> fx = evaluate(vector<vector<float>>(1, vector<float>(MAX_INPUT_COUNT, x)));
        if (fx.empty()) {
            mChart->setVisibility(false);
            return;
        }

        float ySpan = (yBounds.y - yBounds.x);

        float yFactor = (((fx.at(0).at(0)) - yBounds.x) / ySpan);
        points.push_back({yFactor});
    }

    mPointCache = points;
}

void ZNodeView::updateChart1D2X() {
    int chartRes = mChart->getResolution();

    mChart->computeChartBounds();
    vec2 xBounds = mChart->getXBounds();
    vec2 yBounds = mChart->getYBounds();

    vector<vector<float>> points;
    for (int i = 0; i < mChart->getResolution(); i++) {
        float factor = (float) i / (float) chartRes;
        float x = mix(xBounds.x, xBounds.y, factor);
        vector<vector<float>> inVec = {vector<float>(MAX_INPUT_COUNT, x), vector<float>(MAX_INPUT_COUNT, x)};

        vector<vector<float>> fx = evaluate(inVec);
        if (fx.empty()) {
            mChart->setVisibility(false);
            return;
        }

        float ySpan = (yBounds.y - yBounds.x);
        float yFactor = (((fx.at(0).at(0)) - yBounds.x) / ySpan);
        float yFactor2;

        if (fx.size() > 1) {
            yFactor2 = (((fx.at(1).at(0)) - yBounds.x) / ySpan);
        } else {
            yFactor2 = (((fx.at(0).at(1)) - yBounds.x) / ySpan);
        }
        points.push_back({yFactor, yFactor2});
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
                if (socketType.at(0).at(i) == VAR) {
                    mSocketsIn.at(i)->setBackgroundColor(mVariableColor);
                } else if (socketType.at(0).at(i) == CON) {
                    mSocketsIn.at(i)->setBackgroundColor(mConstantColor);
                } else if (socketType.at(0).at(i) == ENUM) {
                    mSocketsIn.at(i)->setBackgroundColor(mEnumColor);
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
                if (socketType.at(1).at(i) == VAR) {
                    mSocketsOut.at(i)->setBackgroundColor(mVariableColor - darkenVec);
                } else if (socketType.at(0).at(i) == CON)  {
                    mSocketsOut.at(i)->setBackgroundColor(mConstantColor - darkenVec);
                } else if (socketType.at(0).at(i) == ENUM) {
                    mSocketsOut.at(i)->setBackgroundColor(mEnumColor - darkenVec);
                }
            }
        }
    }


    // Always set max height to be large enough to fit all sockets
    if (socketCount.x > 0) {
        setMaxHeight(std::max((int) mSocketsIn.at(socketCount.x - 1)->getOffsetY() +
                              SOCKET_SIZE + MIN_MARGIN, getMaxHeight()));
    }


    if (getChartType(type) == LINE_2D) {
        mChart->setInputType(getChartType(getType()));
    } else {
        mChart->setInputType(getChartType(getType()));
    }

    mNameLabel->setText(getName(mType));
    mOutputLabel->setVisibility(isOutputLabelVisible(mType));
    setBackgroundColor(getNodeColor(mType));

    if (isOutputLabelVisible(mType)) {
        setOutputLabel(mConstantValueOutput.at(0));
        mOutputLabel->setBackgroundColor(getNodeColor(mType));
    }

    if (getSocketCount().x == 0) {
        mNameLabel->setXOffset(MIN_MARGIN);
    }

    if (getSocketCount().x > 1) {
        mChart->setMarginLeft(CHART_SIDE_MARGIN_WIDE);
    }

    if (getSocketCount().y > 1) {
        mChart->setMarginRight(MIN_MARGIN);
    }

    if (getChartType(mType) == LINE_1D_2X) {
        mChart->setLineCount(2);
    }

    int i = 0;
    for (float cValue : getDefaultInput(mType)) {
        mConstantValueInput.at(i) = cValue;
        i++;
    }

    vector<int> magnitude = getDefaultMagnitude(type);
    int magIndex = 0;
    for (int & k : mConstantMagnitudeInput) {
        if (magnitude.size() == 1) {
            k = magnitude.at(0);
        } else {
            if (magIndex < magnitude.size()) {
                k = magnitude.at(magIndex);
            }
        }

        magIndex++;
    }

    int buttonIndex = 0;
    for (const string& buttonName : getButtonNames()) {
        int margin = 2;
        ZButton* button = new ZButton(buttonName, this);
        button->setMaxWidth(50);
        button->setMaxHeight(20);
        button->getLabel()->offsetBy(0, -3);
        button->setXOffset(120 + ((button->getMaxWidth() + margin) * buttonIndex));
        button->setOnClick(getButtonCallback(buttonIndex));
        button->setElevation(1.0);
        buttonIndex++;
    }

}

vector<ZView *> ZNodeView::getSocketsIn() {
    return mSocketsIn;
}

vector<ZView *> ZNodeView::getSocketsOut() {
    return mSocketsOut;
}

void ZNodeView::onMouseEvent(int button, int action, int mods, int sx, int sy) {
    ZView::onMouseEvent(button, action, mods, sx, sy);

    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1) {
        if (isMouseInBounds(mOutputLabel) &&
            mOutputLabel->getVisibility()) {
            if (mShowMagnitudeView != nullptr) {
                mShowMagnitudeView(mOutputLabel, this, 0, false, mConstantValueOutput.at(0), "");
            }
        } else {
            int socketIndex = 0;
            for (ZView* inputSocket : mSocketsIn) {
                if (isMouseInBounds(inputSocket) && mInputIndices.at(socketIndex).empty() &&
                        inputSocket->getVisibility() && getSocketType().at(0).at(socketIndex) == CON) {
                    mShowMagnitudeView(mOutputLabel, this, socketIndex, true,
                            mConstantValueInput.at(socketIndex), getSocketNames().at(socketIndex));
                } else if (isMouseInBounds(inputSocket) && mInputIndices.at(socketIndex).empty() &&
                       inputSocket->getVisibility() && getSocketType().at(0).at(socketIndex) == ENUM) {
                    mShowEnumView(mOutputLabel, this, socketIndex, true,
                                       mConstantValueInput.at(socketIndex),
                                       getSocketNames().at(socketIndex), getEnumNames(socketIndex));
                }
                socketIndex++;
            }

        }
    }

    if (isMouseInBounds(mChart) &&
        ((middleMouseIsDown() && shiftKeyPressed()) ||
        (rightMouseIsDown() && shiftKeyPressed()))) {
    }
}

void ZNodeView::setShowMagPickerListener(
        function<void(ZView *sender, ZNodeView *node, int index, bool isInput, float initialValue, string name)> onValueSelect) {
    mShowMagnitudeView = std::move(onValueSelect);
}

void ZNodeView::setShowEnumPickerListener(
        function<void(ZView *sender, ZNodeView *node, int index, bool isInput, float initialValue, string name, vector<string> enumNames)> onValueSelect) {
    mShowEnumView = std::move(onValueSelect);
}

void ZNodeView::setConstantValue(int index, float value, int magnitudeIndex) {
    if (index >= mConstantValueOutput.size()) {
        mOutputLabel->setText("Bad input");
    } else {
        mConstantValueOutput.at(index) = value;
        mConstantMagnitudeOutput.at(index) = magnitudeIndex;
        setOutputLabel(mConstantValueOutput.at(0));
        invalidateNodeRecursive();
    }
}

void ZNodeView::setConstantValueInput(int index, float value, int magnitudeIndex) {
    if (index >= mConstantValueInput.size()) {
        mOutputLabel->setText("Bad input");
    } else {
        mConstantValueInput.at(index) = value;
        mConstantMagnitudeInput.at(index) = magnitudeIndex;
        invalidateNodeRecursive();
    }
}


vector<vector<float>> ZNodeView::evaluate(vector<vector<float>> x) {
    return evaluate(std::move(x), nullptr);
}

vector<vector<float>> ZNodeView::evaluate(vector<vector<float>> x, ZNodeView* root) {
    ivec2 size = getSocketCount();
    vector<vector<float>> output;
    if (size.x == 0) {
        output = compute(x, mType);
    }

    if (size.x > 0) {
        vector<vector<float>> summedInputs = vector<vector<float>>(x.size(), vector<float>(MAX_INPUT_COUNT, 0));
        if (mInputIndices.empty()) {
            return output;
        }


        // Loop over input sockets
        for (int i = 0; i < size.x; i++) {

            const vector<pair<ZNodeView *, int>> &inputs = mInputIndices.at(i);

            // If inputs are connected evaluate recursively, otherwise use the specified input.
            if (!inputs.empty()) {
                // Summing all inputs is useful for dot products.

                // Loop over all inputs on a single socket
                for (pair<ZNodeView *, int> input : inputs) {
                    vector<vector<float>> recurOutput;
                    if (root == nullptr) {
                        recurOutput = input.first->evaluate(x, this);
                    } else {
                        recurOutput = input.first->evaluate(x, root);
                    }
                    for (int d = 0; d < summedInputs.size(); d++) {
                        // It's possible a previous node on the stack has too few inputs.
                        // When that happens display an error message.
                        if (recurOutput.empty()) {
                            mOutputLabel->setText("Bad input");
                            mOutputLabel->setTextColor(red);
                            return vector<vector<float>>();
                        } else {
                            if (d < recurOutput.size() && input.second < recurOutput.at(d).size()) {
                                summedInputs.at(d).at(i) += recurOutput.at(d).at(input.second);
                            }
                        }
                    }
                }

            } else {

                // Check that the passed input vector dimension matches
                // the number of input sockets on the node. If not
                // display an error message.
                if (x.at(0).size() <= size.x) {
                    //mOutputLabel->setText(to_string(size.x) + " inputs needed, got " + to_string(x.size()));
                    mOutputLabel->setTextColor(white);
                    return vector<vector<float>>();
                } else {
                    for (int d = 0; d < summedInputs.size(); d++) {
                        // Use the default input when nothing is connected to a constant socket
                        if (getSocketType().at(0).at(i) == VAR) {
                            summedInputs.at(d).at(i) = x.at(d).at(i);
                        } else if (getSocketType().at(0).at(i) == CON) {
                            // By default constants have no imaginary component
                            if (d == REAL) {
                                summedInputs.at(d).at(i) = mConstantValueInput.at(i);
                            } else {
                                summedInputs.at(d).at(i) = 0.0;
                            }
                        } else if (getSocketType().at(0).at(i) == ENUM) {
                            summedInputs.at(d).at(i) = mConstantMagnitudeInput.at(i);
                        }
                    }
                }
            }
        }

        output = compute(summedInputs, mType);
    }
    return output;

}

void ZNodeView::onWindowChange(int windowWidth, int windowHeight) {
    ZView::onWindowChange(windowWidth, windowHeight);

    int newRes = (int) (getWidth() / 1.0);
    if (abs(newRes - mChart->getResolution()) > CHART_RES_THRESHOLD) {
        if (getChartResolutionMode(getType()) == ADAPTIVE) {
            mChart->setResolution(newRes);
        }
        invalidateSingleNode();
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

void ZNodeView::setMaxWidth(int width) {
    ZView::setMaxWidth(width);

    updateLabelVisibility();
}

void ZNodeView::copyParameters(ZNodeView* node) {
   setOffset(node->getOffset());
   setMaxWidth(node->getMaxWidth());
   setMaxHeight(node->getMaxHeight());
   onWindowChange(getWidth(), getHeight());
   mConstantValueOutput = node->mConstantValueOutput;
   mConstantValueInput = node->mConstantValueInput;
   mConstantMagnitudeInput = node->mConstantMagnitudeInput;
   mConstantMagnitudeOutput = node->mConstantMagnitudeOutput;
   setOutputLabel(mConstantValueOutput.at(0));
}

void ZNodeView::setOutputLabel(float output) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(mMagPrecision.at(mConstantMagnitudeOutput.at(0))) << output;
    mOutputLabel->setText(ss.str());
}

void ZNodeView::updateLabelVisibility() {
    if (getMaxHeight() < LABEL_THRESHOLD || getMaxWidth() < LABEL_THRESHOLD_X) {
        mXMinLabel->setVisibility(false);
        mXMaxLabel->setVisibility(false);
        mYMinLabel->setVisibility(false);
        mYMaxLabel->setVisibility(false);
    } else {
        mXMinLabel->setVisibility(true);
        mXMaxLabel->setVisibility(true);
        mYMinLabel->setVisibility(true);
        mYMaxLabel->setVisibility(true);
    }
}

void ZNodeView::onCursorPosChange(double x, double y) {
    ZView::onCursorPosChange(x, y);
    if (isMouseInBounds(this)) {
        if (getRelativeMouse().x < SOCKET_SIZE + SOCKET_SIZE) {
            int index = 0;
            if (getSocketCount().x > 0) {
                for (const string &name : getSocketNames()) {

                    if (index >= mSocketInLabels.size()) {
                        ZLabel *label = new ZLabel(name, this);
                        label->setTextColor(darkGrey);
                        label->setMaxWidth(100);
                        label->setClippingEnabled(false);
                        mSocketInLabels.push_back(label);
                    }

                    mSocketInLabels.at(index)->drawText();
                    mSocketInLabels.at(index)->setXOffset((int) -mSocketInLabels.at(index)->getFirstLineWidth());
                    mSocketInLabels.at(index)->setYOffset((int) mSocketsIn.at(index)->getOffsetY());

                    mSocketInLabels.at(index)->onWindowChange(getWindowWidth(), getWindowHeight());
                    mSocketInLabels.at(index)->bringToFront();


                    mSocketInLabels.at(index)->setVisibility(true);
                    index++;
                }
            }
        } else {
            hideSocketLabels();
        }
    }
}

void ZNodeView::onGlobalMouseUp(int key) {
    ZView::onGlobalMouseUp(key);
    if (getRelativeMouse().x >= SOCKET_SIZE + SOCKET_SIZE || getRelativeMouse().x < 0) {
        hideSocketLabels();
    }
}

void ZNodeView::hideSocketLabels() {
    if (!mouseIsDown()) {
        for (ZLabel *label : mSocketInLabels) {
            label->setVisibility(false);
        }
    }
}

void ZNodeView::onMouseOver() {
    ZView::onMouseOver();
}

void ZNodeView::onMouseLeave() {
    ZView::onMouseLeave();
    hideSocketLabels();
}
