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

class ZNodeView : public ZView {
public:

    enum Type {
        SIN,
        COS,
        TAN,
        EXP,
        SQRT,
        POW,
        ABS,
        GAUSSIAN,
        MORLET,
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        POLY,
        C, // Constant value
        CI, // Constant value imaginary
        X,
        Y,
        Z, // Complex variable
        T, // Time
        MIN,
        MAX,
        FILE,
        FFT,
        IFFT,
        HARTLEY,
        LAPLACE,
        LAPLACE_S, // symbolic laplace
        FIRST_DIFF,
        DOT,
        CROSS,
        CHART_2D,
        COMBINE,
        SPLIT,
        HEAT_MAP,
        NEURAL_CORE,
        SIGMOID,
        TANH,
        COMMENT,
        GROUP,
        GROUP_IN,
        GROUP_OUT,
        LAST // Fake enum to allow easy iteration
    };

    ZNodeView(ZNodeView::Type type);

    ZNodeView(float maxWidth, float maxHeight, ZView *parent);

    void setShowMagPickerListener(
            function<void(ZView *sender, ZNodeView *node, int socketIndex,
                          bool isInput, float initialValue, string name)> onValueSelect);

    void setInvalidateListener(function<void(ZNodeView *node)> listener);

    enum SocketType {
        NONE,
        CON,
        VAR,
        VAR_Z,
        ENUM,
        NN,
        SYMBOLIC,
        GROUP_SOCKET,
        TEXT

    };

    enum ChartResMode {
        ADAPTIVE,
        STATIC
    };

    enum ChartType {
        LINE_1D,
        LINE_1D_2X,
        LINE_2D,
        IMAGE,
        TEXT_FIELD
    };

    //////////////////////////////////////////////////
    ///// Start node definitions /////////////////////
    //////////////////////////////////////////////////

    vector<vector<SocketType>> NONE_TYPE = {vector<SocketType>(MAX_INPUT_COUNT, NONE),
            vector<SocketType>(MAX_INPUT_COUNT, NONE)};
    vector<vector<SocketType>> mSocketType = NONE_TYPE;

    vector<SocketType> mGroupInSockets = vector<SocketType>(MAX_INPUT_COUNT, CON);
    vector<SocketType> mGroupOutSockets = vector<SocketType>(MAX_OUTPUT_COUNT, VAR);

    vector<vector<SocketType>> getSocketType() {
        mGroupInSockets.at(0) = VAR;

        if (mSocketType.at(0).at(0) == NONE) {
            switch (mType) {
                case POLY: return {{VAR, CON, CON, CON, CON}, {VAR, CON, CON}};
                case SIN:
                case COS: return {{VAR, CON, CON}, {VAR, CON, CON}};
                case TAN:
                case ABS:
                case EXP:
                case SIGMOID:
                case TANH:
                case SQRT: return {{VAR}, {VAR, CON, CON}};
                case POW: return {{VAR, VAR}, {VAR, CON, CON}};
                case GAUSSIAN: return {{VAR, CON, CON}, {VAR, CON, CON}};
                case MORLET: return {{VAR, CON, CON, CON, CON}, {VAR, CON, CON}};
                case ADD:
                case SUBTRACT:
                case MULTIPLY:
                case DIVIDE: return {{CON, CON}, {VAR, CON, CON}};
                case C:
                case CI: return {{}, {CON}};
                case X:
                case Y: return {{}, {VAR, CON, CON}};
                case Z: return {{}, {VAR, CON, CON}};
                case T: return {{CON, CON}, {VAR}};
                case FILE: return {{}, {VAR, CON, CON}};
                case IFFT: return {{VAR, VAR, CON, CON}, {VAR, VAR, CON, CON}};
                case FFT: return {{VAR, VAR, CON, CON}, {VAR, CON, CON}};
                case HARTLEY: return {{VAR, VAR, CON, CON}, {VAR, CON, CON}};
                case LAPLACE: return {{VAR, VAR_Z, CON, CON, CON, CON}, {VAR, CON, CON}};
                case FIRST_DIFF: return {{VAR, VAR}, {VAR, CON, CON}};
                case DOT: return {{CON, CON}, {VAR, CON, CON}};
                case CROSS: return {{VAR, VAR, VAR, VAR}, {VAR, VAR, CON, CON}};
                case CHART_2D: return {{VAR, VAR, CON}, {VAR, VAR, CON, CON}};
                case HEAT_MAP: return {{VAR, CON, CON}, {VAR, CON, CON}};
                case LAPLACE_S: return {{VAR, VAR_Z, CON, CON}, {VAR, CON, CON}};
                case COMBINE: return {{VAR, VAR}, {VAR}};
                case SPLIT: return {{VAR}, {VAR, VAR}};
                case NEURAL_CORE: return {{VAR, VAR, CON, CON, CON, ENUM, ENUM}, {VAR, CON, CON}};
                case GROUP: return {mGroupInSockets, mGroupOutSockets};
                case GROUP_IN: return {{},{mGroupInSockets}};
                case GROUP_OUT: return {{mGroupOutSockets}, {}};
                case MIN:
                case MAX: return {{VAR, VAR}, {VAR, CON, CON}};
                case COMMENT: return {{}, {}};
                case LAST: return {{NONE}, {NONE}};
            }
        }
        return mSocketType;
    }

    ivec2 mSocketCount = ivec2(0);

    /**
     * First value is input count, second value is output count. If input count is zero node
     * is an input node like a constant or number range. If output count is zero the node is
     * an output node like a plot or label.
     * @return Vector with node socket input and output count
     */
    ivec2 getSocketCount() {

        if (mSocketCount == ivec2(0)) {
            mSocketCount = ivec2(getSocketType().at(0).size(),
                                 getSocketType().at(1).size());

            if (isDynamicSockets(mType)) {
                mSocketCount = glm::min(mSocketCount, ivec2(1));
            }
        }
        return mSocketCount;
    }

    static bool isDynamicSockets(Type type) {
        switch (type) {
            case GROUP_IN:
            case GROUP_OUT:
            case GROUP:
                return true;
            default:
                return false;
        }
    }

    constexpr static float const isVar = 0.0;
    static vector<float> getDefaultInput(Type type) {
        switch (type) {
            case POLY:
                return {0.0, 0.0, 0.0, 0.0};
            case ADD:
            case SUBTRACT:
                return {0.0, 0.0};
            case MULTIPLY:
            case DIVIDE:
                return {1.0, 1.0};
            case SIN:
            case COS:
                return {0.0, 1.0, 1.0};
            case GAUSSIAN:
                // X, width, height
                return {0.0, 1.0, 1.0};
            case MORLET:
                return {0.0, 1.0, 1.0, 0.0, 1.0};
            case IFFT:
                return {0.0, 0.0, 1.0, 1.0};
            case FFT:
            case HARTLEY:
                return {0.0, 0.0, 0.0, 1.0};
            case LAPLACE:
                return {0.0, 0.0, 0.0, 1.0, -1.0, 1.0};
            case CHART_2D:
                return {0.0, 0.0, 100};
            case DOT:
                return {1.0, 1.0};
            case HEAT_MAP:
                return {0.0, -1.0, 1.0};
            case LAPLACE_S:
                return {0.0, 0.0, -1.0, 1.0};
            case NEURAL_CORE:
                return {0.0, 0.0, 0.05, -5, -1, 0, 0};
            case GROUP:
                return {0.0};
            default:
                return vector<float>(MAX_INPUT_COUNT, 0.0);
        }
    }

    static vector<int> getDefaultMagnitude(Type type) {
        switch (type) {
            case SIN:
            case COS:
                return {7};
            case NEURAL_CORE:
                return {0, 0, 5, 6, 6, 1, 0, 0};
            default:
                return {6};
        }
    }

    vector<string> getSocketNames() {
        switch (mType) {
            case T:
                return {"Speed", ""};
            case POLY:
                return {"X", "A", "BX", "CX^2", "DX^3"};
            case ADD:
            case SUBTRACT:
            case MULTIPLY:
            case DIVIDE:
                return {"", ""};
            case SIN:
            case COS:
                return {"X", "Frequency", "Height"};
            case GAUSSIAN:
                return {"X", "Width", "Height"};
            case MORLET:
                return {"X", "Width", "Height", "Offset", "Frequency"};
            case IFFT:
                return {"X", "A", "Window Start", "Window Size"};
            case HARTLEY:
            case FFT:
                return {"X", "A", "Window Start", "Window Size"};
            case LAPLACE:
                return {"X", "A", "Window Start", "Window Size", "Z Min", "Z Max"};
            case CHART_2D:
                return {"X", "A", "Resolution"};
            case HEAT_MAP:
                return {"X", "Z Min", "Z Max"};
            case LAPLACE_S:
                return {"X", "Z", "Z Min", "Z Max"};
            case NEURAL_CORE:
                return {"Training Data", "X", "Step Size", "Window Start", "Window Width", "Optimizer", "Activation"};
            case MIN:
            case MAX:
                return {"A", "B"};
            default:
                return {" ", " ", " ", " ", " ", " "};
        }
    }

    vector<string> getEnumNames(int socketIndex) {
        switch (mType) {
            case NEURAL_CORE: {
                switch (socketIndex) {
                    case 5:
                        return {"RMSProp", "Adagrad", "Momentum", "Grad Decent",};
                    case 6:
                        return {"TANH", "SIGMOID", "RELU"};
                    default:
                        return vector<string>(1, " ");
                }

            }
            default:
                return vector<string>(1, " ");
        }

    }

    static string getName(Type type) {
        switch (type) {
            case POLY:
                return "polynomial";
            case SIN:
                return "sin";
            case COS:
                return "cos";
            case TAN:
                return "tan";
            case ABS:
                return "abs";
            case EXP:
                return "exp";
            case SQRT:
                return "sqrt";
            case POW:
                return "^";
            case GAUSSIAN:
                return "gaussian";
            case MORLET:
                return "wavelet";
            case ADD:
                return "+";
            case SUBTRACT:
                return "-";
            case MULTIPLY:
                return "*";
            case DIVIDE:
                return "/";
            case C:
                return "c";
            case CI:
                return "ci";
            case X:
                return "x";
            case Y:
                return "y";
            case Z:
                return "z";
            case T:
                return "t";
            case FILE:
                return "file";
            case IFFT:
                return "ifft";
            case FFT:
                return "fft";
            case HARTLEY:
                return "hartley";
            case LAPLACE:
                return "ztransform";
            case LAPLACE_S:
                return "laplace";
            case FIRST_DIFF:
                return "diff";
            case DOT:
                return "dot";
            case CROSS:
                return "cross";
            case CHART_2D:
                return "chart2d";
            case HEAT_MAP:
                return "heat";
            case COMBINE:
                return "combine";
            case SPLIT:
                return "split";
            case NEURAL_CORE:
                return "neural";
            case SIGMOID:
                return "sigmoid";
            case TANH:
                return "tanh";
            case GROUP:
                return "group";
            case MIN:
                return "min";
            case MAX:
                return "max";
            case GROUP_IN:
                return "in";
            case GROUP_OUT:
                return "out";
            case COMMENT:
                return "comment";
            case LAST:
                return "none";
        }
    }

    static vec2 getNodeSize(Type type) {
        switch (type) {
            case C:
            case CI:
                return vec2(80, 20);
            case CHART_2D:
            case HEAT_MAP:
            case FFT:
            case HARTLEY:
            case IFFT:
                return vec2(200, 200);
            case LAPLACE:
            case LAPLACE_S:
                return vec2(400, 400);
            case NEURAL_CORE:
                return vec2(300, 100);
            case GROUP_IN:
            case GROUP_OUT:
                return vec2(120, 70);
            default:
                return vec2(80, 70);
        }
    }

    static vec4 getChartBounds(Type type) {
        switch (type) {
            case FFT:
            case IFFT:
            case HARTLEY:
                return vec4(0,10,-5,5);
            default:
                return (vec4(-5,5,-5,5));
        }
    }

    static bool isOutputLabelVisible(Type type) {
        switch (type) {
            case C:
            case CI:
                return true;
            default:
                return false;
        }
    }

    vector<string> getButtonNames() {
        switch (mType) {
            case NEURAL_CORE:
                return {"Train", "Reset", "To String"};
            case GROUP_IN:
                return {"+", "-"};
            case GROUP_OUT:
                return {"+", "-"};
            case T:
                return {"Pause"};
            default:
                return {};
        }
    }

    function<void(ZView *sender)> getButtonCallback(int index) {
        switch (mType) {
            case NEURAL_CORE: {
                switch (index) {
                    case 0: {
                        return [this](ZView* sender) {
                            // Train the network
                            trainNN(sender);
                        };
                    }
                    case 1: {
                        return [this](ZView *sender) {
                            mMlModel->resetNetwork();
                        };
                    }
                    case 2: {
                        return [this](ZView *sender) {
                            cout<< mMlModel->toFunctionString() << endl;
                        };
                    }
                    default:
                        return nullptr;

                }

            }
            case GROUP_IN: {
                switch (index) {
                    // Increment
                    case 0: {
                        return [this](ZView *sender) {
                            ivec2 count = mSocketCount + ivec2(0,1);
                            setSocketCount(count);
                        };
                    }
                    // Decrement
                    case 1: {
                        return [this](ZView *sender) {
                            ivec2 count = mSocketCount - ivec2(0,1);
                            setSocketCount(count);
                        };
                    }
                    default:
                        return nullptr;

                }
            }
            case GROUP_OUT: {
                switch (index) {
                    // Increment
                    case 0: {
                        return [this](ZView *sender) {
                            ivec2 count = mSocketCount + ivec2(1,0);
                            setSocketCount(count);
                        };
                    }
                    // Decrement
                    case 1: {
                        return [this](ZView *sender) {
                            ivec2 count = mSocketCount - ivec2(1,0);
                            setSocketCount(count);
                        };
                    }
                    default:
                        return nullptr;

                }
            }
            case T: {
                return [this](ZView *sender) {

                };
            }
            default:
                return nullptr;
        }
    }

    static vec4 getSocketColor(SocketType type) {
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

    static vec4 getNodeColor(Type type) {
        switch (type) {
            case C:
            case CI:
                return getSocketColor(CON);
            case X:
            case Y:
                return getSocketColor(VAR);
            case Z:
                return getSocketColor(VAR_Z);
            case NEURAL_CORE:
            case TANH:
            case SIGMOID:
                return getSocketColor(NN);
            case LAPLACE_S:
                return getSocketColor(SYMBOLIC);
            case GROUP:
            case GROUP_IN:
            case GROUP_OUT:
                return getSocketColor(GROUP_SOCKET);
            case COMMENT:
                return getSocketColor(TEXT);
            default:
                return vec4(1);
        }
    }

    static ChartResMode getChartResolutionMode(Type type) {
        switch (type) {
            default:
                return ADAPTIVE;
            case IFFT:
                return ADAPTIVE;
            case CHART_2D:
            case FFT:
            case HARTLEY:
                return STATIC;
        }
    }

    static ChartType getChartType(Type type) {
        switch (type) {
            default:
                return LINE_1D_2X;
            case HARTLEY:
            case FFT:
            case IFFT:
                return LINE_1D_2X;
            case CHART_2D:
                return LINE_2D;
            case LAPLACE:
            case LAPLACE_S:
            case HEAT_MAP:
                return IMAGE;
            case COMMENT:
                return TEXT_FIELD;
        }
    }

    static bool showInDrawer(Type type) {
        switch (type) {
            case GROUP_IN:
            case GROUP_OUT:
                return false;
            default:
                return true;
        }
    }

    vector<vector<float>> compute(vector<vector<float>> x, ZNodeView::Type type, vector<vector<float>> rootInput);

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
                mMlCache.clear();
                for (vector<float> output : outputs) {
                    mMlCache.emplace_back(output.at(0), output.at(0));
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

        if (inputSocket.empty() && !getSocketType().empty()) {
            if (getSocketType().at(0).at(socketIndex) == VAR) {
                summedInputs = x;
            } else if (getSocketType().at(0).at(socketIndex) == CON) {
                // By default constants have no imaginary component
                if (var == REAL) {
                    summedInputs = mConstantValueInput.at(socketIndex);
                } else {
                    summedInputs = 0.0;
                }
            } else if (getSocketType().at(0).at(socketIndex) == ENUM) {
                summedInputs = mConstantMagnitudeInput.at(socketIndex);
            }
        }

        return summedInputs;
    }

    vector<vector<float>> evaluate(vector<vector<float>> x, vector<vector<float>> rootInput);

    vector<vector<float>> evaluate(vector<vector<float>> x, ZNodeView *root, vector<vector<float>> rootInput);

    void setType(Type type);

    Type getType() {
        return mType;
    }

    vector<vector<pair<ZNodeView *, int>>> mInputIndices;
    vector<vector<pair<ZNodeView *, int>>> mOutputIndices;

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

    void setEditorDeletionInterface(std::function<void(ZNodeView*, int)> interface) {
        mEditorDeleteInterface = std::move(interface);
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

    void readyForRecycle(bool ready) {
        mReadyForRecycle = ready;
    }

    bool isReadyForRecycle() {
        return mReadyForRecycle;
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
    * Internal group node graph
    */
    ZNodeView* mGroupInput = nullptr;
    ZNodeView* mInputProxy = nullptr;
    ZNodeView* mGroupParent = nullptr;
    set<ZNodeView*> mGroupNodes;

    vector<ZButton*> mButtons;
    ZNodeView* mGroupOutput = nullptr;
    Type mType = ADD;
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
     * The view interface is used to allow the node to
     * add other views to the parent editor.
     */
    function<void(ZNodeView*, bool)> mEditorInterface = nullptr;

    /**
     * The editor delete interface is used when the node whats to delete itself.
     * This interface allows the node to inform the editor about it's deletion.
     */
    function<void(ZNodeView*, int)> mEditorDeleteInterface = nullptr;

    /**
     * Number of line segments on the chart
     */
    // Todo: remove  these
    ZLineChart *mChart = nullptr;
    vector<vector<float>> mPointCache;
    vector<float> mPointCache1D;

    vector<float> mConstantValueOutput = vector<float>(MAX_OUTPUT_COUNT, 0.0);
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

    vector<complex<float>> mFftCache;
    vector<complex<float>> mMlCache;

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

    vector<vector<float>> computeLaplaceHeadless(vector<vector<float>> x, vector<vector<float>> rootInput);

    void onCreate();

    void refreshView(Type &type);

    void initializeGroup();

    vector<vector<float>> sumAllInputs(vector<vector<float>> x, ZNodeView *root, vector<vector<float>> rootInput);

    void onSizeChange() override;
};


#endif //ZPATH_ZNODEVIEW_H
