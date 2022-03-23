
//
// Created by lukas on 10/4/20.
//
#include <utility>
#include <ui/zlabel.h>
#include <ui/zbutton.h>
#include <ui/zcheckbox.h>
#include <utils/zsettingsstore.h>
#include "ui/znodeview.h"
#include <sstream>
#include <iomanip>
#include <mutex>
#include "utils/znodeutil.h"
#include "utils/dataimportutil.h"

ZNodeView::ZNodeView(NodeType* type) : ZView(10, 10){
    mType = type;
    init();
    setType(type);
}


ZNodeView::ZNodeView(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight, parent) {
    init();
}

void ZNodeView::onCreate() {
    ZView::onCreate();
    int buttonIndex = 0;
    int margin = 2;
    int firstLineWidth = (int) mNameLabel->getTextWidth();
    int buttonOffset = firstLineWidth + margin + mNameLabel->getOffsetX();

    for (auto button : mButtons) {
        button->setMaxWidth(button->getLabel()->getTextWidth() / mDP + 18);
        button->setMaxHeight(18);
        button->setYOffset(2);
        button->setXOffset(buttonOffset);
        buttonOffset += button->getMaxWidth();
        button->setElevation(1.0);
        buttonIndex++;
    }
}

void ZNodeView::init() {
    mChart = new ZLineChart(fillParent, fillParent, this);

    mNameLabel = new ZTextField(this);
    mNameLabel->setFocusMode(ZTextField::FocusMode::doubleClick);
    mNameLabel->setTextColor(black);
    mNameLabel->setTextMode(ZTextField::TextMode::field);
    mNameLabel->setCornerRadius(0.0);
    mNameLabel->setBackgroundColor(transparent);
    setBackgroundColor(vec4(0.95, 0.95, 0.95, 1.0));

    mOutputLabel = new ZLabel("", this);
    mOutputLabel->setOffset(vec2(20, 0));
    mOutputLabel->setMaxWidth(40);
    mOutputLabel->setYOffset(1);
    mOutputLabel->setGravity(topRight);

    setOutlineType(outline);
    setLineWidth(2.0);

    float yOffset = 3;
    float margin = 10;

    // Add input sockets
    auto socketType = mType->mSocketType;

    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        auto* socket = new ZView(SOCKET_WIDTH, SOCKET_HEIGHT, this);
        socket->setOffset(2, yOffset + i * (SOCKET_HEIGHT + margin));
        socket->setClickable(false);
        socket->setHighlighColor(vec4(0));
        socket->setTint(vec4(1));
        mSocketsIn.push_back(socket);
    }

    // Add output sockets
    for (int i = 0; i < MAX_OUTPUT_COUNT; i++) {
        auto* socket = new ZView(SOCKET_WIDTH, SOCKET_HEIGHT, this);
        socket->setGravity(topRight);
        socket->setOffset(2, yOffset + i * (SOCKET_HEIGHT + margin));
        socket->setClickable(false);
        mSocketsOut.push_back(socket);
    }

    initializeEdges();

    mChart->setOffset(vec2(0, 10));
    mChart->setChartListener([this](vector<int> x, int lineIndex){
        // Can safely ignore line index for now

        if (mType->mChartType == LINE_1D || mType->mChartType == LINE_1D_2X) {
            if (mPointCache.empty()) {
                return vector<float>(1, 0);
            } else if (x.empty() || mPointCache.size() - 1 < x.at(0) || mPointCache.at(x.at(0)).empty()) {
                return vector<float>(1, 0);
            }
            return vector<float>(1, mPointCache.at(x.at(0)).at(lineIndex));
        } else if (mType->mChartType == LINE_2D) {
            if (mPointCache.empty() || mPointCache.size() <= x.at(0)) {
                return vector<float>(2, 0);
            }
            vector<float> point;
            point.push_back(mPointCache.at(x.at(0)).at(0));
            point.push_back(mPointCache.at(x.at(0)).at(1));

            return point;
        } else if (mType->mChartType == IMAGE) {
            return mPointCache1D;
        }
    });

    mChart->setInvalidateListener([this](){
        invalidateNodeRecursive();
    });
    mChart->resetZoom();
    mChart->resetTmpTransform();

    // Bound labels
    vec4 boundColor = vec4(0.4,0.4,0.4,1.0);
    int boundLabelWidth = 50;
    mXMinLabel = new ZLabel("xmin", mChart);
    mXMinLabel->setMaxWidth(boundLabelWidth);
    mXMinLabel->setGravity(bottomLeft);
    mXMinLabel->setTextColor(boundColor);
    mXMinLabel->setBackgroundColor(whiteFaded);
    mXMinLabel->setCornerRadius(mXMinLabel->getMaxHeight()/2);
    mXMinLabel->setElevation(1.0);
    mXMinLabel->setMargin(vec4(2));
   // mXMinLabel->setOutlineColor(grey3);
    //mXMinLabel->setLineWidth(0.0001);

    mXMaxLabel = new ZLabel("xmax", mChart);
    mXMaxLabel->setMaxWidth(boundLabelWidth);
    mXMaxLabel->setGravity(bottomRight);
    mXMaxLabel->setMargin(vec4(0));
    mXMaxLabel->setXOffset(-0);
    mXMaxLabel->setTextColor(boundColor);
    mXMaxLabel->setBackgroundColor(whiteFaded);
    mXMaxLabel->setCornerRadius(mXMaxLabel->getMaxHeight()/2);
    mXMaxLabel->setElevation(1.0);
    mXMaxLabel->setMargin(vec4(2));
   // mXMaxLabel->setOutlineColor(grey3);
    //mXMaxLabel->setLineWidth(0.0001);

    mYMinLabel = new ZLabel("ymin", mChart);
    mYMinLabel->setMaxWidth(boundLabelWidth);
    mYMinLabel->setGravity(bottomLeft);
    mYMinLabel->setYOffset(20);
    mYMinLabel->setTextColor(boundColor);
    mYMinLabel->setBackgroundColor(whiteFaded);
    mYMinLabel->setCornerRadius(mYMinLabel->getMaxHeight()/2);
    mYMinLabel->setElevation(1.0);
    mYMinLabel->setMargin(vec4(2));
    //mYMinLabel->setOutlineColor(grey3);
    //mYMinLabel->setLineWidth(0.0001);

    mYMaxLabel = new ZLabel("ymax", mChart);
    mYMaxLabel->setMaxWidth(boundLabelWidth);
    mYMaxLabel->setGravity(topLeft);
    mYMaxLabel->setTextColor(boundColor);
    mYMaxLabel->setBackgroundColor(whiteFaded);
    mYMaxLabel->setCornerRadius(mYMaxLabel->getMaxHeight()/2);
    mYMaxLabel->setElevation(1.0);
    mYMaxLabel->setMargin(vec4(2));
   // mYMaxLabel->setOutlineColor(grey3);
   // mYMaxLabel->setLineWidth(0.0001);

    updateLabelVisibility();

    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        if (i >= mSocketInLabels.size()) {
            auto *label = new ZLabel("", this);
            label->setTextColor(grey2);
            label->setMaxWidth(100);
            label->setClippingEnabled(false);
            mSocketInLabels.push_back(label);
        }
    }

}

void ZNodeView::setSocketCount(ivec2 count) {
    mSocketCount = glm::min(count, ivec2(MAX_INPUT_COUNT - 1, MAX_OUTPUT_COUNT - 1));
    refreshView(mType);
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
        if (mType->mChartType == LINE_1D) {
            updateChart1D();
        } else if (mType->mChartType == LINE_2D) {
            updateChart2D();
        } else if (mType->mChartType == LINE_1D_2X) {
            updateChart1D2X();
        } else if (mType->mChartType == IMAGE) {
            mLaplaceCache.clear();

            for (ZNodeView* node : mHeadlessLaplaceNodes) {
                node->setVisibility(false);
                ZNodeUtil::get().deleteNode(node);
            }
            mHeadlessLaplaceNodes.clear();
            updateChartHeatMap();
        }
    }

    mChart->invalidateData();
    mChart->invalidate();

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
    // Run from background thread

    // todo: update this when x and y resolution defined independently
    int xRes = std::min(mChart->getResolution(), mChart->getMaxResolution());
    int yRes = std::min(mChart->getResolution(), mChart->getMaxResolution());

    vec2 xBounds = mChart->getXBounds();
    vec2 yBounds = mChart->getYBounds();

    vector<float> points;

    for (int iy = 0; iy < yRes; iy++) {
        for (int ix = 0; ix < xRes; ix++) {
            float xFactor = (float) ix / (float) xRes;
            float yFactor = (float) iy / (float) yRes;

            float x = mix(xBounds.x, xBounds.y, xFactor);
            float y = mix(yBounds.x, yBounds.y, yFactor);
            vector<vector<float>> inVec = {
                    vector<float>(MAX_INPUT_COUNT, x),
                            vector<float>(MAX_INPUT_COUNT, y)};

            vector<vector<float>> z = evaluate(inVec, inVec);
            if (!z.empty()) {
                points.push_back(z.at(0).at(0));
            }
        }
    }

    mPointCache1D = points;
}

void ZNodeView::updateChart2D() {
    // Run from background thread

    // Data doesn't necessarily go off screen after this number of points.
    // Finding this optimal resolution is potentially complicated.
    float increment = 0.1;
    vec2 xBounds = mChart->getXBounds();
    vec2 yBounds = mChart->getYBounds();
   // vector<vector<float>> inVec = vector<vector<float>>(2, vector<float>(MAX_INPUT_COUNT, 0));
   // evaluate(evaluate(inVec, inVec),inVec);
    vector<vector<float>> points;
    for (int i = 0; i < mChart->getResolution(); i++) {
        float input = (float) i * increment;

        vector<vector<float>> inVec = vector<vector<float>>(2, vector<float>(MAX_INPUT_COUNT, input));
        vector<vector<float>> fx = evaluate(inVec, inVec);
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
    // Run from background thread
    int chartRes = mChart->getResolution();

    vec2 xBounds = mChart->getXBounds();
    vec2 yBounds = mChart->getYBounds();

    vector<vector<float>> points;
    for (int i = 0; i < mChart->getResolution(); i++) {
        float factor = (float) i / (float) chartRes;
        float x = mix(xBounds.x, xBounds.y, factor);
        vector<vector<float>> inVec = vector<vector<float>>(1, vector<float>(MAX_INPUT_COUNT, x));
        vector<vector<float>> fx = evaluate(inVec, inVec);
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
    // Run from background thread

    int chartRes = mChart->getResolution();
    vec2 xBounds = mChart->getXBounds();
    vec2 yBounds = mChart->getYBounds();

    vector<vector<float>> points;
    for (int i = 0; i < mChart->getResolution(); i++) {
        float factor = (float) i / (float) chartRes;
        float x = mix(xBounds.x, xBounds.y, factor);
        vector<vector<float>> inVec = {vector<float>(MAX_INPUT_COUNT, x), vector<float>(MAX_INPUT_COUNT, x)};

        vector<vector<float>> fx = evaluate(inVec, inVec);
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

void ZNodeView::setType(NodeType* type) {
    mSocketType = NONE_TYPE;
    mSocketCount = ivec2(0);

    setMaxWidth(type->mNodeSize.x);
    setMaxHeight(type->mNodeSize.y);
    initializeEdges();

    mOutputLabel->setVisibility(type->mIsOutputLabelVisible);

    setBackgroundColor(type->mColor);

    if (type->mIsOutputLabelVisible) {
        setOutputLabel(mConstantValueOutput.at(0));
    }

    int i = 0;
    for (float cValue : type->mDefaultInput) {
        mConstantValueInput.at(i) = cValue;
        i++;
    }

    vector<int> magnitude = type->mDefaultMagnitude;
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

    for (ZButton* button : mButtons) {
        button->setVisibility(false);
    }

    int buttonIndex = 0;
    int margin = 2;
    int firstLineWidth = (int) mNameLabel->getTextWidth();
    int buttonOffset = firstLineWidth + margin + mNameLabel->getOffsetX();
    for (const string& buttonName : type->mButtonNames) {
        ZButton* button;
        if (mButtons.size() <= buttonIndex) {
            button = new ZButton(buttonName, this);
            mButtons.push_back(button);

        } else {
            button = mButtons.at(buttonIndex);
        }
        button->setOnClick(type->mOnButtonClick.at(buttonIndex));
        button->setMaxWidth(button->getLabel()->getTextWidth() / mDP + 18);
        button->setMaxHeight(18);
        button->setYOffset(2);
        button->setCornerRadius(vec4(8));
        button->setVisibility(true);
        button->setIndexTag(buttonIndex);

        button->computeBounds();
        button->setBackgroundColor(white);
        button->setXOffset(buttonOffset);
        buttonOffset += button->getMaxWidth();

        button->setElevation(1.0);
        buttonIndex++;
    }

    if (type->mIsDropDownVisible) {
        if (mDropDown == nullptr) {
            mDropDown = new ZDropDown(220, 18, {" "}, this);
            mDropDown->setTitle("Choose file...");
            mDropDown->setMarginLeft(25);
            mDropDown->setMarginRight(25);
            mDropDown->setMarginTop(2);
            mDropDown->setOnOpen([this](){
                vector<string> list = DataStore::get().getFileNameList();
                mDropDown->setItems(list);
            });
            mDropDown->setOnItemChange([this](int item){

            });

        } else {
            mDropDown->setVisibility(true);
        }
    } else {
        if (mDropDown != nullptr) {
            mDropDown->setVisibility(false);
        }
    }

    refreshView(type);
    mNameLabel->setTextColor(getBackgroundColor().getTextColor());
    mOutputLabel->setTextColor(getBackgroundColor().getTextColor());
    if (type->mChartType == LINE_1D_2X) {
        mChart->setLineCount(2);
    }

    mChart->setDefaultMat(type->mChartBounds);
    mChart->resetZoom();
}

void ZNodeView::refreshView(NodeType* type) {
    ivec2 socketCount = type->mSocketCount;

    auto socketType = type->mSocketType;
    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        if (i >= socketCount.x) {
            mSocketsIn.at(i)->setVisibility(false);
        } else {
            mSocketsIn.at(i)->setVisibility(true);
            if (i < socketCount.x) {
                ZColor color = getSocketColor(socketType.at(0).at(i));
                mSocketsIn.at(i)->setBackgroundColor(color);
            }

            // First socket round corner a little more
            if (i == 0) {
                mSocketsIn.at(i)->setCornerRadius(vec4(3,
                                                       SOCKET_HEIGHT / 2, SOCKET_HEIGHT / 2, 0));
            } else {
                mSocketsIn.at(i)->setCornerRadius(vec4(0,
                                                       SOCKET_HEIGHT / 2, SOCKET_HEIGHT / 2, 0));
            }

        }
    }
    vec4 darkenVec = vec4(0.5,0.4,0.4,0.0);
    for (int i = 0; i < MAX_OUTPUT_COUNT; i++) {
        if (i >= socketCount.y) {
            mSocketsOut.at(i)->setVisibility(false);
        } else {

            mSocketsOut.at(i)->setVisibility(true);
            if (i < socketCount.y) {
                ZColor color = getSocketColor(socketType.at(1).at(i)) - darkenVec;
                mSocketsOut.at(i)->setBackgroundColor(color);
            }
        }

        // First socket round corner a little more
        if (i == 0) {
            mSocketsOut.at(i)->setCornerRadius(
                    vec4(SOCKET_HEIGHT / 2, 3, 0, SOCKET_HEIGHT / 2));
        } else {
            mSocketsOut.at(i)->setCornerRadius(
                    vec4(SOCKET_HEIGHT / 2, 0, 0, SOCKET_HEIGHT / 2));
        }

    }

    // Always set max height to be large enough to fit all sockets
    int inSocketOffset = 0;
    int outSocketOffset = 0;
    if (socketCount.x > 0) {
      inSocketOffset = (int) mSocketsIn.at(socketCount.x - 1)->getOffsetY();
    }
    if (socketCount.y > 0) {
        outSocketOffset = (int) mSocketsOut.at(socketCount.y - 1)->getOffsetY();
    }

    int socketOffset = std::max(inSocketOffset, outSocketOffset);
    setMaxHeight(std::max(socketOffset + SOCKET_HEIGHT + CHART_TOP_MARGIN, (int) getMaxHeight()));

    ChartType chartType = type->mChartType;
    mChart->setInputType(chartType);

    if (chartType == TEXT_FIELD) {
        mChart->setVisibility(false);
        mNameLabel->setFocusMode(ZTextField::FocusMode::doubleClick);
        mNameLabel->setTextMode(ZTextField::TextMode::editor);
        mNameLabel->setMaxHeight(fillParent);
    } else {
        mChart->setVisibility(true);
        mNameLabel->setFocusMode(ZTextField::FocusMode::doubleClick);
        mNameLabel->setTextMode(ZTextField::TextMode::field);
        mNameLabel->setMaxHeight(mNameLabel->getLineHeight());
        mNameLabel->setTitleText(type->mName);
    }

    mNameLabel->setXOffset(22);
    mNameLabel->setYOffset(1);
    mChart->setMargin(vec4(MIN_MARGIN, CHART_TOP_MARGIN, MIN_MARGIN, MIN_MARGIN));

    if (type->mSocketCount.x == 0) {
        mNameLabel->setXOffset(MIN_MARGIN);
    }

    if (type->mSocketCount.x > 1) {
        mChart->setMarginLeft(CHART_SIDE_MARGIN_WIDE);
    }

    if (type->mSocketCount.y > 1) {
        mChart->setMarginRight(MIN_MARGIN);
    }


    int buttonIndex = 0;
    int margin = 2;
    int firstLineWidth = (int) mNameLabel->getTextWidth();
    int buttonOffset = firstLineWidth + margin + mNameLabel->getOffsetX();
    for (const string& buttonName : type->mButtonNames) {

        ZButton* button;
        if (mButtons.size() > buttonIndex) {
            button = mButtons.at(buttonIndex);
        } else {
           break;
        }
        button->setBackgroundColor(white);
        button->setXOffset(buttonOffset);
        buttonOffset += button->getMaxWidth();
        button->onWindowChange(getWindowWidth(), getWindowHeight());
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
                        inputSocket->getVisibility() && mType->mSocketType.at(0).at(socketIndex) == CON) {
                    mShowMagnitudeView(mOutputLabel, this, socketIndex, true,
                            mConstantValueInput.at(socketIndex), mType->mSocketNames.at(socketIndex));
                } else if (isMouseInBounds(inputSocket) && mInputIndices.at(socketIndex).empty() &&
                       inputSocket->getVisibility() && mType->mSocketType.at(0).at(socketIndex) == ENUM) {
                    mShowEnumView(mOutputLabel, this, socketIndex, true,
                                       mConstantValueInput.at(socketIndex),
                                  mType->mSocketNames.at(socketIndex), mType->mEnumNames.at(socketIndex));
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
        function<void(ZView *sender, ZNodeView *node, int index, bool isInput,
                float initialValue, string name)> onValueSelect) {
    mShowMagnitudeView = std::move(onValueSelect);
}

void ZNodeView::setShowEnumPickerListener(
        function<void(ZView *sender, ZNodeView *node, int index, bool isInput,
                float initialValue, string name, vector<string> enumNames)> onValueSelect) {
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

float ZNodeView::getConstantValue(int index) {
    return  mConstantValueOutput.at(index);
}

float ZNodeView::getConstantInput(int index) {
    return mConstantValueInput.at(index);
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

vector<vector<float>> ZNodeView::evaluate(vector<vector<float>> x, vector<vector<float>> rootInput) {
    return evaluate(std::move(x), nullptr, std::move(rootInput));
}

vector<vector<float>>
ZNodeView::sumAllInputs(vector<vector<float>> x, ZNodeView *root, vector<vector<float>> rootInput) {
    ivec2 size = mType->mSocketCount;

    if (mInputIndices.empty()) {
        return x;
    }

    vector<vector<float>> summedInputs = vector<vector<float>>(x.size(), vector<float>(MAX_INPUT_COUNT, 0));
    // Loop over input sockets
    for (int i = 0; i < size.x; i++) {

        const vector<pair<ZNodeView *, int>> &inputs = mInputIndices.at(i);

        // If inputs are connected evaluate recursively, otherwise use the specified input.
        if (!inputs.empty()) {
            // Summing all inputs is useful for dot products.

            // Loop over all inputs on a single socket
            for (pair<ZNodeView *, int> input : inputs) {

                if (input.first != nullptr) {
                    vector<vector<float>> recurOutput;
                    if (root == nullptr) {
                        recurOutput = input.first->evaluate(x, this, rootInput);
                    } else {
                        recurOutput = input.first->evaluate(x, root, rootInput);
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
            }

        } else {

            // Check that the passed input vector dimension matches
            // the number of input sockets on the node. If not
            // display an error message.
            if (x.at(0).size() <= size.x) {
                //mOutputLabel->setText(to_string(size.x) + " inputs needed, got " + to_string(x.size()));
                mOutputLabel->setTextColor(getBackgroundColor().getTextColor());
                return vector<vector<float>>();
            } else {
                auto socketType = mType->mSocketType;
                auto sinput = socketType.at(0);
                if (!sinput.empty() && sinput.size() > i) {
                    for (int d = 0; d < summedInputs.size(); d++) {
                        // Use the default input when nothing is connected to a constant socket
                        if (sinput.at(i) == VAR) {
                            summedInputs.at(REAL).at(i) = x.at(REAL).at(i);
                            summedInputs.at(IMAG).at(i) = 0;
                        } else if (sinput.at(i) == VAR_Z) {
                            summedInputs.at(REAL).at(i) = x.at(REAL).at(i);
                            summedInputs.at(IMAG).at(i) = x.at(IMAG).at(i);
                        } else if (sinput.at(i) == CON) {
                            // By default constants have no imaginary component
                            if (d == REAL) {
                                summedInputs.at(d).at(i) = mConstantValueInput.at(i);
                            } else {
                                summedInputs.at(d).at(i) = 0.0;
                            }
                        } else if (sinput.at(i) == ENUM) {
                            summedInputs.at(d).at(i) = mConstantMagnitudeInput.at(i);
                        }
                    }
                }
            }
        }
    }

    return summedInputs;
}

vector<vector<float>> ZNodeView::evaluate(vector<vector<float>> x, ZNodeView *root, vector<vector<float>> rootInput) {
    if (root == this) {
        return vector<vector<float>>(MAX_INPUT_COUNT, vector<float>(MAX_INPUT_COUNT, 0));
    }
    ivec2 size = mType->mSocketCount;
    if (size.x == 0) {
        x = compute(x, mType, rootInput);
    }

    if (size.x > 0) {
        vector<vector<float>> summedInputs = sumAllInputs(x, root, rootInput);
        x = compute(summedInputs, mType, rootInput);
    }
    return x;

}

void ZNodeView::onWindowChange(int windowWidth, int windowHeight) {
    ZView::onWindowChange(windowWidth, windowHeight);

    int newRes = (int) (getWidth() / 1.0);
    if (abs(newRes - mChart->getResolution()) > CHART_RES_THRESHOLD) {
        if (getType()->mAdaptiveRes) {
            mChart->setResolution(newRes);
        }
        invalidateSingleNode();
    }
}

void ZNodeView::draw() {
    ZView::draw();

    // GLSL mode computes the graph in the draw step.
    if (ZSettings::get().getCompDevice() == glsl) {
        mChart->setGpuMode(true);
    } else {
        mChart->setGpuMode(false);
    }

    if (getType()->mName == "t") {
        invalidateNodeRecursive();
    }
}

void ZNodeView::clearInvalidateNode() {
    //setBackgroundColor(white);  // Turn this to another color to debug invalidation logic.
    mInvalid = false;
    mRecursiveInvalidate = false;
}

/**
 * Only invalidate this node, don't update children.
 */
void ZNodeView::invalidateSingleNode() {
    //setBackgroundColor(blue); // Turn this to another color to debug invalidation logic.
    if (getVisibility()) {
        mInvalid = true;
        if (mInvalidateListener != nullptr) {
            mInvalidateListener(this);
            mChart->computeChartBounds();
        }
    }
}

void ZNodeView::invalidateForDelete() {
    if (mInvalidateListener != nullptr) {
        mInvalidateListener(this);
    }
}

/**
 * Invalidate this node and all child nodes
 */
void ZNodeView::invalidateNodeRecursive() {
    invalidateSingleNode();

    if (!mRecursiveInvalidate) {

        mRecursiveInvalidate = true;
        for (const vector<pair<ZNodeView *, int>> &outputSocket : mOutputIndices) {
            for (pair<ZNodeView *, int> child : outputSocket) {
                if (child.first == nullptr) {
                    continue;
                }
                if (!child.first->isInvalid()) {
                    child.first->invalidateNodeRecursive();
                }
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
    float original = getMaxWidth();
    ZView::setMaxWidth(width);
    updateLabelVisibility();
    if (abs(original - width) < 0.01) {
        onSizeChange();
    }
}

void ZNodeView::onSizeChange() {
    ZView::onSizeChange();
    updateLabelVisibility();
}

void ZNodeView::copyParameters(ZNodeView* node) {
   setOffset(node->getOffset());
   setMaxWidth(node->getMaxWidth());
   setMaxHeight(node->getMaxHeight());
   setText(node->getText());
   onWindowChange(getWidth(), getHeight());
   mConstantValueOutput = node->mConstantValueOutput;
   mConstantValueInput = node->mConstantValueInput;
   mConstantMagnitudeInput = node->mConstantMagnitudeInput;
   mConstantMagnitudeOutput = node->mConstantMagnitudeOutput;
   setOutputLabel(mConstantValueOutput.at(0));

   setSocketCount(node->getType()->mSocketCount);
}

void ZNodeView::setOutputLabel(float output) {
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

    vec2 mouse = getRelativeMouse();
    int size = SOCKET_HEIGHT + SOCKET_HEIGHT;
    if (mouse.x < size && mouse.x > -size && mouse.y > 0 && mouse.y < getMaxHeight()) {
        int index = 0;
        if (mType->mSocketCount.x > 0) {
            for (const string &name : mType->mSocketNames) {
                mSocketInLabels.at(index)->setText(name);
                mSocketInLabels.at(index)->setXOffset((int) -mSocketInLabels.at(index)->getTextWidth());
                mSocketInLabels.at(index)->setYOffset((int) mSocketsIn.at(index)->getOffsetY());
                mSocketInLabels.at(index)->setVisibility(true);
                index++;
            }
        }

        onWindowChange(getWindowWidth(), getWindowHeight());
        invalidate();
    } else {
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

vector<vector<float>> ZNodeView::computeLaplaceHeadless(vector<vector<float>> x, vector<vector<float>> rootInput) {

    if (mHeadlessLaplaceNodes.empty()) {
        ZNodeView* root = this;
        string exp;
        if (!mInputIndices.empty() && !mInputIndices.at(0).empty()) {
            exp = ZNodeUtil::get().graphToExpString(root, false);
        } else {
            exp = ZNodeUtil::get().graphToExpString(root, false);
        }

        string laplace = "laplace(" + exp + ")";
        string result = ZUtil::replace(CasUtil::get().evaluate(laplace), "\n", "");
        string zResult = ZUtil::replace(result, "x", "z");
        vector<ZNodeView*> headless = ZNodeUtil::get().expStringToGraph(zResult);
        for (auto lnode : headless) {
            mHeadlessLaplaceNodes.push_back(lnode);
        }

    }
    return mHeadlessLaplaceNodes.at(0)->evaluate(std::move(x), rootInput);
}

vector<vector<float>>
ZNodeView::compute(vector<vector<float>> x, NodeType* type, vector<vector<float>> rootInput) {
    vec2 chartBound = mChart->getXBounds();
    float chartWidth = chartBound.y - chartBound.x;
    vector<vector<float>> output;
    for (uint d = 0; d < x.size(); d++) {
        vector<float> in = x.at(d);
        vector<float> out;

        vector<vector<float>> result = type->mCompute(x, rootInput, mCache, chartBound.x, chartWidth, this);
        return result;

//        switch (type) {
//            case POLY: {
//                float in0 = x.at(REAL).at(0);
//                float term0 = x.at(REAL).at(1);
//                float term1 = x.at(REAL).at(2);
//                float term2 = x.at(REAL).at(3);
//                float term3 = x.at(REAL).at(4);
//
//                float out0 = (term3 * pow(in0, 3)) + (term2 * pow(in0, 2)) + (term1 * in0) + term0;
//
//                x.at(REAL).at(0) = out0;
//                x.at(REAL).at(1) = chartBound.x;
//                x.at(REAL).at(2) = chartWidth;
//
//                x.at(IMAG).at(0) = 0.0;
//                x.at(IMAG).at(1) = chartBound.x;
//                x.at(IMAG).at(2) = chartWidth;
//                return x;
//            }
//            case TAN: {
//                complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
//                complex<float> out0 = tan(in0);
//                return {{out0.real(), chartBound.x, chartWidth},
//                        {out0.imag(), chartBound.x, chartWidth}};
//            }
//            case ABS: {
//                complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
//                complex<float> out0 = abs(in0);
//                return {{out0.real(), chartBound.x, chartWidth},
//                        {out0.imag(), chartBound.x, chartWidth}};
//            }
//            case EXP: {
//                complex<float> comIn = {x.at(REAL).at(0), x.at(IMAG).at(0)};
//                complex<float> comOut = exp(comIn);
//                return {{comOut.real(), chartBound.x, chartWidth},
//                        {comOut.imag(), chartBound.x, chartWidth}};
//            }
//            case SIGMOID: {
//                float in =x.at(REAL).at(0);
//                float out = 1.0 / (1.0 + exp(-in));
//                return {{out, chartBound.x, chartWidth},
//                        {0.0, chartBound.x, chartWidth}};
//            }
//            case TANH: {
//                float in =x.at(REAL).at(0);
//                float out = tanh(in);
//                return {{out, chartBound.x, chartWidth},
//                        {0.0, chartBound.x, chartWidth}};
//            }
//
//            case SQRT: {
//                complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
//                complex<float> out0 = sqrt(in0);
//                return {{out0.real(), chartBound.x, chartWidth},
//                        {out0.imag(), chartBound.x, chartWidth}};
//            }
//            case POW: {
//                complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
//                complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
//                complex<float> out0 = pow(in0, in1);
//                return {{out0.real(), chartBound.x, chartWidth},
//                        {out0.imag(), chartBound.x, chartWidth}};
//            }
//            case GAUSSIAN: {
//                complex<float> in0 = {x.at(REAL).at(0), 0};
//                complex<float> in1 = {x.at(REAL).at(1), 0};
//                complex<float> in2 = {x.at(REAL).at(2), 0};
//                complex<float> two = {2.0, 0};
//                complex<float> out0 = (in2 * exp(-pow(in0, two) / pow(two * in1, two)));
//                return {{out0.real(), chartBound.x, chartWidth},
//                        {out0.imag(), chartBound.x, chartWidth}};
//            }
//            case MORLET: {
//                auto real = (float) (
//                        cos(in.at(0) * in.at(4)) * // sinusoid
//                        (in.at(2) * exp(-pow(in.at(0) - in.at(3), 2) /
//                        pow(2 * in.at(1), 2))));
//
//                auto imaginary = (float) (
//                        sin(in.at(0) * in.at(4)) * // sinusoid
//                        (in.at(2) * exp(-pow(in.at(0) - in.at(3), 2) /
//                        pow(2 * in.at(1), 2))));
//                return {{real,      chartBound.x, chartWidth},
//                        {imaginary, chartBound.x, chartWidth}};
//            }
//
//            case ADD: {
//                complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
//                complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
//                complex<float> out0 = in0 + in1;
//                return {{out0.real(), chartBound.x, chartWidth},
//                        {out0.imag(), chartBound.x, chartWidth}};
//            }
//            case SUBTRACT:
//                out = {in.at(0) - in.at(1), chartBound.x, chartWidth};
//                break;
//            case MULTIPLY: {
//                complex<float> a = {x.at(REAL).at(0), x.at(IMAG).at(0)};
//                complex<float> b = {x.at(REAL).at(1), x.at(IMAG).at(1)};
//
//                auto result = a * b;
//                return {{result.real(), chartBound.x, chartWidth},
//                        {result.imag(), chartBound.x, chartWidth}};
//            }
//            case DIVIDE: {
//                float a = x.at(REAL).at(1);
//                float b = x.at(IMAG).at(1);
//                float c = x.at(REAL).at(0);
//                float e = x.at(IMAG).at(0);
//
//                float denom = pow(a, 2) + pow(b, 2);
//
//                float r = (c * a + b * e) / denom;
//                float img = (a * d - c * e) / denom;
//                return {{r, chartBound.x, chartWidth},
//                        {{img, chartBound.x, chartWidth}}};
//            }
//            case C:
//                x.at(REAL) = mConstantValueOutput;
//                x.at(IMAG).at(0) = 0;
//                return x;
//
//            case CI:
//                x.at(REAL).at(0) = 0;
//                x.at(IMAG) = mConstantValueOutput;
//                return x;
//            case T: {
//                x.at(REAL).at(0) = glfwGetTime() * x.at(REAL).at(0);
//                return x;
//            }
//            case X:
//                x.at(REAL).at(0) = x.at(REAL).at(0);
//                x.at(REAL).at(1) = chartBound.x;
//                x.at(REAL).at(2) = chartWidth;
//
//                x.at(IMAG).at(0) = 0;
//                x.at(IMAG).at(1) = chartBound.x;
//                x.at(IMAG).at(2) = chartWidth;
//                return x;
//            case Y:
//                return {{x.at(IMAG).at(0),chartBound.x, chartWidth},
//                        {0, chartBound.x, chartWidth}};
//            case Z:
//                return {{x.at(REAL).at(0), chartBound.x, chartWidth},
//                        {x.at(IMAG).at(0), chartBound.x, chartWidth}};
//            case FILE: {
//                int fileIndex = mDropDown->getSelectedItem();
//
//                float point = DataStore::get().getDataAtIndex(fileIndex, x.at(REAL).at(0));
//                return {{point, chartBound.x, chartWidth},
//                        {0, chartBound.x, chartWidth}};
//                break;
//            }
//            case FFT: {
//                auto fft = computeFft(in.at(1), in.at(2), in.at(3));
//                return {{fft.first,  chartWidth, in.at(3)},
//                        {fft.second, chartWidth, in.at(3)}};
//            }
//            case IFFT: {
//                auto fft = computeInverseFft(in.at(1), in.at(2), in.at(3));
//                return {{fft.first,  chartBound.x, chartWidth},
//                        {fft.second, chartBound.x, chartWidth}};
//            }
//            case HARTLEY: {
//                auto fft = computeFft(in.at(1), in.at(2), in.at(3));
//                return {{sqrt(pow(fft.first, 2.0f) + pow(fft.second, 2.0f)),
//                         chartWidth, in.at(3)},
//                        {0.0, chartWidth, in.at(3)}};
//            }
//            case LAPLACE: {
//              //  mChart->setZBound(vec2(x.at(REAL).at(4), x.at(REAL).at(5)));
//                // Static resolution for now
//                auto laplace = computeLaplace(x.at(REAL).at(1),
//                                              x.at(IMAG).at(1), in.at(2), in.at(3),
//                                              mChart->getMaxResolution());
//
//                x.at(REAL).at(0) = laplace.first;
//                return x;
//            }
//            case LAPLACE_S: {
//
//                vector<vector<float>> sx = computeLaplaceHeadless(
//                        {vector<float>(MAX_INPUT_COUNT, x.at(REAL).at(1)),
//                         vector<float>(MAX_INPUT_COUNT, x.at(IMAG).at(1))}, rootInput);
//
//                mChart->setZBound(vec2(x.at(0).at(2), x.at(0).at(3)));
//                out = {sx.at(0).at(0), chartBound.x, chartWidth};
//                break;
//            }
//            case FIRST_DIFF: {
//                float diff = computeFirstDifference(in.at(0), in.at(1));
//                out = {diff, chartBound.x, chartWidth};
//                break;
//            }
//            case DOT:
//                return {{dot(vec2(x.at(0).at(0), x.at(1).at(0)),
//                             vec2(x.at(0).at(1), x.at(1).at(1))), chartBound.x, chartWidth}};
//            case CROSS:
//                return {
//                        {dot(vec2(x.at(0).at(0), x.at(1).at(0)),
//                             vec2(x.at(0).at(1), x.at(1).at(1))), chartBound.x, chartWidth}
//                };
//                break;
//            case CHART_2D: {
//                mChart->setResolution(100);
//
//                x.at(REAL).at(0) = in.at(0);
//                x.at(REAL).at(1) = in.at(1);
//                x.at(REAL).at(2) = chartBound.x;
//                x.at(REAL).at(3) = chartWidth;
//
//                x.at(IMAG).at(0) = 0.0;
//                x.at(IMAG).at(1) = 0.0;
//                x.at(IMAG).at(2) = 0.0;
//                x.at(IMAG).at(3) = 0.0;
//                return x;
//            }
//
//            case HEAT_MAP: {
//                mChart->setZBound(vec2(x.at(0).at(1), x.at(0).at(2)));
//                out = {x.at(0).at(0), chartBound.x, chartWidth};
//                break;
//            }
//            case COMBINE: {
//                return {{x.at(REAL).at(0)},
//                        {x.at(REAL).at(1)}};
//            }
//            case SPLIT: {
//                return {{x.at(REAL).at(0), x.at(IMAG).at(0)},
//                        {NAN, NAN}};
//            }
//            case NEURAL_CORE: {
//                if (mMlModel == nullptr) {
//                    initializeNNModel();
//                }
//
//                float returnValue;
//                if (mMlModel->getTrainingInProgress()) {
//                    vec2 thisChartBounds = mChart->getXBounds();
//                    float span = thisChartBounds.y - thisChartBounds.x;
//                    float inX = ((x.at(REAL).at(1) - thisChartBounds.x) / span) * mMlCache.size();
//                    if (span > 0) {
//                        int xIndex = 0;
//                        if (inX >= 0 && (inX) < mMlCache.size() && !mMlCache.empty()) {
//                            xIndex = (int) inX;
//                            complex<float> y = mMlCache.at(xIndex);
//                            returnValue = y.real();
//                        }
//                    }
//
//                } else {
//                    mMlModel->setInput(x.at(REAL).at(1), 0);
//                    mMlModel->compute();
//                    returnValue = mMlModel->getOutputAt(0);
//                }
//
//                return {{returnValue, chartBound.x, chartWidth},
//                        {x.at(REAL).at(0),      chartBound.x, chartWidth}};
//            }
//            case NEURAL_GROUP:
//            case GROUP: {
//                initializeGroup();
//                return mGroupOutput->evaluate(rootInput, nullptr, rootInput);
//            }
//            case GROUP_IN: {
//                if (mInputProxy != nullptr) {
//                    return mInputProxy->sumAllInputs(x, nullptr, vector<vector<float>>());
//                } else {
//                    return x;
//                }
//            }
//            case GROUP_OUT: {
//                return x;
//            }
//            case MIN:
//                return {{std::min(x.at(REAL).at(0), x.at(REAL).at(1)), chartBound.x, chartWidth},
//                        {std::min(x.at(IMAG).at(0), x.at(IMAG).at(1)), chartBound.x, chartWidth}};
//            case MAX:
//                return {{std::max(x.at(REAL).at(0), x.at(REAL).at(1)), chartBound.x, chartWidth},
//                        {std::max(x.at(IMAG).at(0), x.at(IMAG).at(1)), chartBound.x, chartWidth}};
//            case LAST:
//                break;
//        }

        output.push_back(out);
    }

    return output;
}

void ZNodeView::initializeGroup() {
    if (!isDeleted()) {
        if (mGroupInput == nullptr) {
            mGroupInput = ZNodeUtil::get().newNode(
                    ZNodeDefStore::get().getNodeType("in"));
            mGroupInput->setInputProxy(this);
            mGroupNodes.insert(mGroupInput);
            if (mEditorInterface != nullptr) {
                mEditorInterface(mGroupInput, true);
            }
        }

        if (mGroupOutput == nullptr) {
            mGroupOutput = ZNodeUtil::get().newNode(
                    ZNodeDefStore::get().getNodeType("out"));
            mGroupNodes.insert(mGroupOutput);
            if (mEditorInterface != nullptr) {
                mEditorInterface(mGroupOutput, true);
            }
        }

        if (getVisibility()) {
            mGroupOutput->setVisibility(false);
            mGroupInput->setVisibility(false);
        }
    }
}
