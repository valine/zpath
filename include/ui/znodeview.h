//
// Created by lukas on 10/4/20.
//

#ifndef ZPATH_ZNODEVIEW_H
#define ZPATH_ZNODEVIEW_H

static const int SOCKET_WIDTH = 15;
static const int SOCKET_HEIGHT = 15;
static const int MAX_INPUT_COUNT = 8;
static const int MAX_OUTPUT_COUNT = 8;
static const int CHART_RES_THRESHOLD = 4;
static const int CHART_SIDE_MARGIN_WIDE = 20;
static const int MIN_MARGIN = 3;
static const int CHART_TOP_MARGIN = 10;
static const int DEFAULT_MAGNITUDE = 6;
static const int LABEL_THRESHOLD = 120;
static const int LABEL_THRESHOLD_X = 160;
static const int REAL = 0;
static const int IMAG = 1;
using namespace std;

#include <utility>
#include <vector>
#include "zview.h"
#include "zchart.h"
#include "zlinechart.h"
#include <complex.h>
#include <utils/zfft.h>
#include <array>
#include "neuralcore/mlmodel.h"
#include "zbutton.h"
#include <set>
#include "utils/casutil.h"
#include "ztextfield.h"
#include "utils/zutil.h"
#include "zdropdown.h"
#include "ui/nodetype.h"
#include "ui/charttype.h"
#include "ui/sockettype.h"

class FuncIn;

class ZNodeView : public ZView {
public:

    ZNodeView(NodeType* type);
    ZNodeView(float maxWidth, float maxHeight, ZView *parent);

    void setShowMagPickerListener(
            function<void(ZView *sender, ZNodeView *node, int socketIndex,
                          bool isInput, float initialValue, string name)> onValueSelect);

    void setInvalidateListener(function<void(ZNodeView *node)> listener);

    ivec2 mSocketCount = ivec2(0);

    /**
     * First value is input count, second value is output count. If input count is zero node
     * is an input node like a constant or number range. If output count is zero the node is
     * an output node like a plot or label.
     * @return Vector with node socket input and output count
     */
    ivec2 getSocketCount() {

        if (mSocketCount == ivec2(0)) {
            mSocketCount = ivec2(mType->mSocketCount.x,
                                 mType->mSocketCount.y);

            if (mType->mIsDynamicSocket) {
                mSocketCount = glm::min(mSocketCount, ivec2(1));
            }
        }
        return mSocketCount;
    }

    static ZColor getSocketColor(SocketType type) {
        const vec4 mVariableColor = vec4(1, 0.611956, 0.052950, 1);
        const vec4 mConstantColor = vec4(1, 0.437324, 0.419652, 1);
        const vec4 mEnumColor = vec4(0.038825, 0.538225, 0.048049, 1.000000);
        const vec4 mNNColor = vec4(0.023195, 0.223442, 0.538225, 1.000000);
        const vec4 mSymbolicColor = vec4(0.122923, 0.061397, 0.314665, 1.000000);
        const vec4 mVariableZColor = vec4(0.848084, 0.215260, 0.077509, 1.000000);
        const vec4 mGroupColor = vec4(0.04, 0.04, 0.04, 1.000000);
        const vec4 mCommentColor = vec4(0.455475, 0.288508, 0.171833, 1.000000);

        switch (type) {
            case CON:
                return mConstantColor;
            case VAR:
                return mVariableColor;
            case ENUM:
                return mEnumColor;
            case NN:
                return mNNColor;
            case VAR_Z:
                return mVariableZColor;
            case SYMBOLIC:
                return mSymbolicColor;
            case GROUP_SOCKET:
                return mGroupColor;
            case TEXT:
                return mCommentColor;
            default:
                return mVariableColor;
        }
    }

    bool operator < (ZNodeView* node) {
        return (getOffsetY() < node->getOffsetY());
    }

    void offsetByYRecursive(float offset) {
        offsetBy(0, offset);
        for (ZNodeView* child : getChildren()) {
            child->offsetByYRecursive(offset);
        }
    }

    vector<vector<float>> compute(vector<vector<float>> x, NodeType* type,
                                  vector<vector<float>> rootInput);

    ///////////////////////////////////////////
    ///////// End node definition /////////////
    ///////////////////////////////////////////
    void initializeNNModel() {
        vector<int> heights = {3, 3, 3};
        int width = heights.size();

        int inputs = 1;
        int outputs = 1;
        int batchSize = 50;
        float stepSize = 0.0001;
        MlModel::Optimizer optimizer = MlModel::RMSPROP;
        Neuron::Activation type = Neuron::TANH;
        vector<Neuron::Activation> activationFunctions = vector<Neuron::Activation>(width, type);

        mMlModel = new MlModel(width, heights, inputs, outputs,
                               batchSize, stepSize, optimizer, activationFunctions);
        mMlModel->resetNetwork();
    }

    void trainNN(ZView *sender) {
        if (mMlModel == nullptr) {
            initializeNNModel();
        }

        vector<pair<vector<double>, vector<double>>> trainingData;
        int samples = 200;

        int socketIndex = 0;
        int dimenIndex = 0;

        auto fres = (float) samples;

        float windowStart = sumInputs(0.0, 3, 0);
        float windowSize = sumInputs(0.0, 4, 0);

        // When window size is less than zero use this nodes chart window size
        vec2 chartBound = mChart->getXBounds();
        float chartWidth = chartBound.y - chartBound.x;
        if (windowSize < 0) {
            windowSize = chartWidth;
            windowStart = chartBound.x;
        }

        for (int i = 0; i < samples; i++) {
            float t = (((float) i / fres) * windowSize) + windowStart;
            float summedInput = sumInputs(t, socketIndex, dimenIndex);
            trainingData.push_back({{t},
                                    {summedInput}});
        }

        mMlModel->setStepSize(sumInputs(0.0, 2, 0));

        int activationIndex = sumInputs(0, 6, 0);
        switch (activationIndex) {
            case 0: {
                mMlModel->setActivationFunction(Neuron::TANH);
                break;
            }
            case 1: {
                mMlModel->setActivationFunction(Neuron::SIGMOID);
                break;
            }
            case 2: {
                mMlModel->setActivationFunction(Neuron::RELU);
                break;
            }
        }

        int optmizerIndex = sumInputs(0, 5, 0);
        setOptimizer(optmizerIndex);

        mMlModel->setTrainingData(trainingData);
        mMlModel->computeNormalization();
        mMlModel->setTrainingCallback([this]() {
            int optmizerIndex = sumInputs(0, 5, 0);
            setOptimizer(optmizerIndex);
            mMlModel->setStepSize(sumInputs(0.0, 2, 0));


            vec2 xBound = mChart->getXBounds();
            float span = xBound.y - xBound.x;
            float inc = span / mChart->getResolution();
            vector<vector<float>> job;
            for (float x = xBound.x; x < xBound.y; x += inc) {
                job.push_back({x});

            }
            mMlModel->computeAsync(job, [this](vector<vector<float>> outputs) {
                mCache.clear();
                for (vector<float> output : outputs) {
                    mCache.emplace_back(output.at(0), output.at(0));
                }

                invalidateNodeRecursive();
            });


        });

        if (mMlModel->getTrainingInProgress()) {
            mMlModel->requestStopTraining();
            mButtons.at(sender->getIndexTag())->setText("Train");
        } else {
            mMlModel->trainNetworkAsync(10000);
            mButtons.at(sender->getIndexTag())->setText("Stop");
        }
    }

    void setOptimizer(int optmizerIndex) const {
        switch (optmizerIndex) {
            case 0: {
                mMlModel->setOptimizer(MlModel::RMSPROP);
                break;
            }
            case 1: {
                mMlModel->setOptimizer(MlModel::ADAGRAD);
                break;
            }
            case 2: {
                mMlModel->setOptimizer(MlModel::MOMENTUM);
                break;
            }
            case 3: {
                mMlModel->setOptimizer(MlModel::GD);
                break;
            }

        }
    }

    float computeFirstDifference(float fx, float x) {
        vec2 bound = mChart->getXBounds();
        int index = 0;
        float h = ((bound.y - bound.x) / (float) mChart->getResolution());
        float x2 = x + (h);
        float fxh = sumInputs(x2, index, 0);
        float y = (fxh - fx) / h;
        return y;
    }

    pair<float, float> computeFft(float in, float start, float width) {
        int res = std::max((int) mChart->getXBounds().y, 5);
        mChart->setResolution(res);
        if (mFftCache.empty()) {

            for (int i = 0; i < res * 2; i++) {
                float x = mix(start, start + width, (float) i / (float) (res * 2));
                int socketIndex = 0;
                float summedInput = sumInputs(x, socketIndex, 0);
                float summedInput2 = sumInputs(x, socketIndex, 1);
                if (isnan(summedInput)) {
                    summedInput = 0;
                }
                if (isnan(summedInput2)) {
                    summedInput2 = 0;
                }
                mFftCache.emplace_back(summedInput, summedInput2);
            }

            ZFFt::transform(mFftCache);
        }

        vec2 thisChartBounds = mChart->getXBounds();
        float span = thisChartBounds.y - thisChartBounds.x;

        pair<float, float> returnValue = {NAN, NAN};
        if (span > 0) {
            int xIndex = 0;
            if (in >= 0 && (in) < mFftCache.size() && !mFftCache.empty()) {
                xIndex = (int) in;
                complex<float> y = mFftCache.at(xIndex);
                returnValue = {y.real() / res, y.imag() / res};
            }
        }

        mChart->invalidate();
        return returnValue;
    }

    pair<float, float> computeInverseFft(float in, float fftRes, float windowSize) {
        int res = std::max((int) fftRes, 1);
        if (mFftCache.empty()) {
            for (int i = 0; i < res * 2; i++) {

                int socketIndex = 0;
                int dimenIndexX = 0;
                int dimenIndexY = 1;
                auto inputSocket = mInputIndices.at(0);
                float summedInput = sumInputs(i, socketIndex, dimenIndexX);
                float summedInput2 = sumInputs(i, socketIndex, dimenIndexY);

                mFftCache.emplace_back(summedInput, summedInput2);
            }
            ZFFt::inverseTransform(mFftCache);
        }

        pair<float, float> returnValue = {NAN, NAN};
        uint xIndex = (int) ((in * (mFftCache.size() - 1)) / windowSize);
        if (xIndex >= 0 && !mFftCache.empty() && windowSize > 0 && xIndex < mFftCache.size()) {

            complex<float> y = mFftCache.at(xIndex);
            returnValue = {y.real(), y.imag()};
        }

        mChart->invalidate();
        return returnValue;
    }

    pair<float, float> computeLaplace(float x, float y, float start, float windowSize, int resolution) {

        vec2 xBounds = mChart->getXBounds();
        vec2 yBounds = mChart->getYBounds();

        float xFactor = (x - xBounds.x) / (xBounds.y - xBounds.x);
        float yFactor = (y - yBounds.x) / (yBounds.y - yBounds.x);

        int xi = (int) std::min(std::max(0.0f, resolution * xFactor), (float) resolution - 1.0f);
        int yi = (int) std::min(std::max(0.0f, resolution * yFactor), (float) resolution - 1.0f);

        if (!mLaplaceCache.empty()) {
            //cout << "x: " << xi << " y: " << yi << " size: " << mLaplaceCache.size() << " " << mLaplaceCache.at(0).size() << endl;
            return {mLaplaceCache.at(xi).at(yi), 0};
        }

        auto fres = (float) resolution;
        int socketIndex = 0;
        int dimenIndex = 0;

        vector<float> timeDomain;
        for (int i = 0; i < resolution; i++) {
            float t = (((float) i / fres) * windowSize) + start;
            float summedInput = sumInputs(t, socketIndex, dimenIndex);
            timeDomain.push_back(summedInput);
        }

        mLaplaceCache = vector<vector<float>>(resolution, vector<float>(resolution, 0));
        for (int ei = 0; ei < resolution; ei++) {
            for (int fi = 0; fi < resolution; fi++) {

                float freq = ((float) mix(yBounds.x, yBounds.y, (float) fi / fres)) * 2.0 * M_PI;
                float expo = ((float) mix(xBounds.x, xBounds.y, (float) ei / fres));
                complex<float> sum = {0, 0};

                for (int t = 0; t < resolution; t++) {
                    float time = ((t / fres) * windowSize) + start;
                    complex<float> sC = {-expo, -freq};
                    complex<float> timeC = {time, 0.0};
                    complex<float> fxC = {timeDomain.at(t), 0};
                    complex<float> output = fxC * exp(sC * timeC);
                    sum += output;
                }

                mLaplaceCache.at(ei).at(fi) = sum.real() / resolution;
                //  mLaplaceCache.at(ei).at(fi) = sqrt(pow(sum.real(), 2.0) + pow(sum.imag(), 2.0)) / resolution;
            }
        }

        return {mLaplaceCache.at(xi).at(yi), 0};
    }

    float sumInputs(float x, int socketIndex, int var) {
        float summedInputs = 0.0;
        auto inputSocket = mInputIndices.at(socketIndex);
        for (auto singleInput : inputSocket) {
            vector<vector<float>> inVec = vector<vector<float>>(2,vector<float>(MAX_INPUT_COUNT, x));
            summedInputs += singleInput.first->evaluate(inVec,inVec).at(
                    var).at(singleInput.second);
        }

        if (inputSocket.empty() && !mType->mSocketType.empty()) {
            if (mType->mSocketType.at(0).at(socketIndex) == VAR) {
                summedInputs = x;
            } else if (mType->mSocketType.at(0).at(socketIndex) == CON) {
                // By default constants have no imaginary component
                if (var == REAL) {
                    summedInputs = mConstantValueInput.at(socketIndex);
                } else {
                    summedInputs = 0.0;
                }
            } else if (mType->mSocketType.at(0).at(socketIndex) == ENUM) {
                summedInputs = mConstantMagnitudeInput.at(socketIndex);
            }
        }

        return summedInputs;
    }

    vector<vector<float>> evaluate(vector<vector<float>> x, vector<vector<float>> rootInput);

    vector<vector<float>> evaluate(vector<vector<float>> x, ZNodeView *root, vector<vector<float>> rootInput);

    void setType(NodeType* type);

    NodeType* getType() {
        return mType;
    }

    vector<vector<pair<ZNodeView *, int>>> mInputIndices;
    vector<vector<pair<ZNodeView *, int>>> mOutputIndices;

    bool mChildrenValid = false;
    bool mParentsValid = false;
    vector<ZNodeView*> mChildren;
    vector<ZNodeView*> mParents;


    vector<ZView *> getSocketsIn();

    vector<ZView *> getSocketsOut();

    void updateChart();

    void onWindowChange(int windowWidth, int windowHeight) override;

    void setConstantValue(int index, float value, int magnitudeIndex);

    void setConstantValueInput(int index, float value, int magnitudeIndex);

    int getInputMagnitude(int index) {
        return mConstantMagnitudeInput.at(index);
    }

    int getOutputMagnitude(int index) {
        return mConstantMagnitudeOutput.at(index);
    }

    void invalidateSingleNode();

    void invalidateNodeRecursive();

    void initializeEdges();

    void copyParameters(ZNodeView *node);

    void setMaxWidth(int width) override;

    vector<ZNodeView*> getChildren();

    //////////////////
    /// Neural core integration

    MlModel *mMlModel = nullptr;

    //////////

    void setShowEnumPickerListener(
            function<void(ZView *, ZNodeView *, int, bool, float, string, vector<string>)> onValueSelect);

    float getConstantValue(int index);

    float getConstantInput(int index);

    vector<float> getConstantInputs() {
        return mConstantValueInput;
    }

    vector<ZNodeView*> mHeadlessLaplaceNodes = vector<ZNodeView*>();

    void setEditorInterface(std::function<void(ZNodeView*, bool)> interface) {
        mEditorInterface = std::move(interface);
    }

    function<void(ZNodeView*, int)> getEditorDeletionInterface() {
        return mEditorDeleteInterface;
    }

    std::function<void(ZNodeView*, bool)> getAddNodeInterface() {
        return mEditorInterface;
    }

    void setIsDeleted(bool isDeleted) {
        mIsDeleted = isDeleted;
        mReadyForRecycle = false;
    }

    bool isDeleted() {
        return mIsDeleted;
    }

    ZNodeView* getGroupInput() {
        return mGroupInput;
    }

    ZNodeView* getGroupOutput() {
        return mGroupOutput;
    }

    void addGroupNode(ZNodeView* node) {
        mGroupNodes.insert(node);
        node->mGroupParent = this;
        node->mIsPartOfGroup = true;
    }

    set<ZNodeView*> getGroupNodes() {
        return mGroupNodes;
    }

    void setSocketCount(ivec2 count);

    void invalidateForDelete();

    /**
     * The input proxy node is used within the group node
     * to pull input from outside the group.
     * @param nodeView
     */
    void setInputProxy(ZNodeView* nodeView) {
        mInputProxy = nodeView;
    }

    ZNodeView* getGroupParent() {
        return mGroupParent;
    }

    void setGroupParent(ZNodeView* node) {
        mGroupParent = node;
    }

    void setProjectID(int id) {
        mProjectID = id;
    }

    int getProjectID() {
        return mProjectID;
    }

    /**
     * Returns the node name / text.
     * @return Node text
     */
    string getText() {
        return mNameLabel->getText();
    }

    void setText(string text) {
        mNameLabel->setText(std::move(text));
    }

    /**
    * Internal group node graph
    */
    bool mIsPartOfGroup = false;
    ZNodeView* mGroupInput = nullptr;
    ZNodeView* mInputProxy = nullptr;
    ZNodeView* mGroupParent = nullptr;
    set<ZNodeView*> mGroupNodes;
    vector<ZButton*> mButtons;
    ZNodeView* mGroupOutput = nullptr;
    NodeType* mType = nullptr;

/**
 * The view interface is used to allow the node to
 * add other views to the parent editor.
 */
function<void(ZNodeView*, bool)> mEditorInterface = nullptr;
    vector<float> mConstantValueOutput = vector<float>(MAX_OUTPUT_COUNT, 0.0);

    void initializeGroup();

    vector<vector<float>> sumAllInputs(vector<vector<float>> x, ZNodeView *root, vector<vector<float>> rootInput);

    ZLineChart *mChart = nullptr;
    ZDropDown* mDropDown = nullptr;

    vector<vector<float>> computeLaplaceHeadless(vector<vector<float>> x, vector<vector<float>> rootInput);

    vector<ZNodeView *> getParents();

private:
    bool mInvalid = true;
    bool mRecursiveInvalidate = false;
    bool mIsDeleted = false;
    bool mReadyForRecycle = false;

    vector<ZView *> mSocketsIn;
    vector<ZView *> mSocketsOut;

    vector<ZLabel *> mSocketInLabels;
    vector<ZLabel *> mSockOutLabels;

    ZLabel *mOutputLabel = nullptr;
    ZTextField *mNameLabel = nullptr;

    ZLabel *mXMinLabel = nullptr;
    ZLabel *mXMaxLabel = nullptr;
    ZLabel *mYMinLabel = nullptr;
    ZLabel *mYMaxLabel = nullptr;

    /**
     * The editor delete interface is used when the node whats to delete itself.
     * This interface allows the node to inform the editor about it's deletion.
     */
    function<void(ZNodeView*, int)> mEditorDeleteInterface = nullptr;

    /**
     * Number of line segments on the chart
     */
    vector<vector<float>> mPointCache;
    vector<float> mPointCache1D;

    vector<float> mConstantValueInput = vector<float>(MAX_INPUT_COUNT, 0.0);

    // Center magnitude is at index 6
    vector<int> mConstantMagnitudeOutput = vector<int>(MAX_OUTPUT_COUNT, DEFAULT_MAGNITUDE);
    vector<int> mConstantMagnitudeInput = vector<int>(MAX_INPUT_COUNT, DEFAULT_MAGNITUDE);

    vector<int> mMagPrecision = {8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0};

    function<void(ZView *sender, ZNodeView *node, int socketIndex,
                  bool isInput, float initialValue, string name)> mShowMagnitudeView = nullptr;

    function<void(ZView *sender, ZNodeView *node, int socketIndex,
                  bool isInput, float initialValue, string name, vector<string> enumNames)> mShowEnumView = nullptr;

    function<void(ZNodeView *node)> mInvalidateListener = nullptr;

    vector<complex<float>> mCache = vector<complex<float>>();
    vector<complex<float>> mFftCache;
    vector<vector<float>> mLaplaceCache;

    int mProjectID = -1;

    void onMouseEvent(int button, int action, int mods, int sx, int sy) override;

    void clearInvalidateNode();

    bool isInvalid();

    void updateChart1D();

    void updateChart2D();

    void updateLabelVisibility();

    void updateChart1D2X();

    void setOutputLabel(float output);

    void updateChartHeatMap();

    void onCursorPosChange(double x, double y);

    void hideSocketLabels();

    void init();

    void draw();

    void onCreate();

    void refreshView(NodeType* type);

    void onSizeChange() override;

};


#endif //ZPATH_ZNODEVIEW_H
