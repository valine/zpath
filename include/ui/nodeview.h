//
// Created by lukas on 10/4/20.
//

#ifndef ZPATH_NODEVIEW_H
#define ZPATH_NODEVIEW_H

static const int SOCKET_SIZE = 15;
static const int MAX_INPUT_COUNT = 8;
static const int MAX_OUTPUT_COUNT = 4;
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

#include <vector>
#include "zview.h"
#include "zchart.h"
#include "zlinechart.h"
#include <complex.h>
#include <utils/zfft.h>
#include <array>
#include "zlabel.h"
#include "neuralcore/mlmodel.h"
#include "zbutton.h"

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
        FILE,
        FFT,
        IFFT,
        HARTLEY,
        LAPLACE,
        FIRST_DIFF,
        DOT,
        CROSS,
        CHART_2D,
        COMBINE,
        SPLIT,
        HEAT_MAP,
        NEURAL_CORE,
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
        ENUM,
        NN,
    };

    enum ChartResMode {
        ADAPTIVE,
        STATIC
    };

    enum ChartType {
        LINE_1D,
        LINE_1D_2X,
        LINE_2D,
        IMAGE
    };


    vector<vector<float>> compute(vector<vector<float>> x, Type type) {
        vec2 chartBound = mChart->getXBounds();
        float chartWidth = chartBound.y - chartBound.x;
        vector<vector<float>> output;
        for (uint d = 0; d < x.size(); d++) {
            vector<float> in = x.at(d);

            vector<float> out;
            switch (type) {
                case POLY: {
                    float in0 = x.at(REAL).at(0);
                    float term0 = x.at(REAL).at(1);
                    float term1 = x.at(REAL).at(2);
                    float term2 = x.at(REAL).at(3);
                    float term3 = x.at(REAL).at(4);

                    float out0 = (term3 * pow(in0, 3)) + (term2 * pow(in0, 2)) + (term1 * in0) + term0;

                    x.at(REAL).at(0) = out0;
                    x.at(REAL).at(1) = chartBound.x;
                    x.at(REAL).at(2) = chartWidth;

                    x.at(IMAG).at(0) = 0.0;
                    x.at(IMAG).at(1) = chartBound.x;
                    x.at(IMAG).at(2) = chartWidth;
                    return x;
                }
                case SIN: {
                    complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
                    complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
                    complex<float> in2 = {x.at(REAL).at(2), x.at(IMAG).at(2)};
                    complex<float> out0 = sin(in0 * in1) * in2;
                    return {{out0.real(), chartBound.x, chartWidth},
                            {out0.imag(), chartBound.x, chartWidth}};
                }
                case COS: {
                    complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
                    complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
                    complex<float> in2 = {x.at(REAL).at(2), x.at(IMAG).at(2)};
                    complex<float> out0 = cos(in0 * in1) * in2;
                    return {{out0.real(), chartBound.x, chartWidth},
                            {out0.imag(), chartBound.x, chartWidth}};
                }
                case TAN: {
                    complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
                    complex<float> out0 = tan(in0);
                    return {{out0.real(), chartBound.x, chartWidth},
                            {out0.imag(), chartBound.x, chartWidth}};
                }
                case ABS: {
                    complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
                    complex<float> out0 = abs(in0);
                    return {{out0.real(), chartBound.x, chartWidth},
                            {out0.imag(), chartBound.x, chartWidth}};
                }
                case EXP: {
                    complex<float> comIn = {x.at(REAL).at(0), x.at(IMAG).at(0)};
                    complex<float> comOut = exp(comIn);
                    return {{comOut.real(), chartBound.x, chartWidth},
                            {comOut.imag(), chartBound.x, chartWidth}};
                }
                case SQRT: {
                    complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
                    complex<float> out0 = sqrt(in0);
                    return {{out0.real(), chartBound.x, chartWidth},
                            {out0.imag(), chartBound.x, chartWidth}};
                }
                case POW: {
                    complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
                    complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
                    complex<float> out0 = pow(in0, in1);
                    return {{out0.real(), chartBound.x, chartWidth},
                            {out0.imag(), chartBound.x, chartWidth}};
                }
                case GAUSSIAN: {
                    complex<float> in0 = {x.at(REAL).at(0), 0};
                    complex<float> in1 = {x.at(REAL).at(1), 0};
                    complex<float> in2 = {x.at(REAL).at(2), 0};
                    complex<float> two = {2.0, 0};
                    complex<float> out0 = (in2 * exp(-pow(in0, two) / pow(two * in1, two)));
                    return {{out0.real(), chartBound.x, chartWidth},
                            {out0.imag(), chartBound.x, chartWidth}};
                }
                case MORLET: {
                    auto real = (float) (
                            cos(in.at(0) * in.at(4)) * // sinusoid
                            (in.at(2) * exp(-pow(in.at(0) - in.at(3), 2) / pow(2 * in.at(1), 2))));

                    auto imaginary = (float) (
                            sin(in.at(0) * in.at(4)) * // sinusoid
                            (in.at(2) * exp(-pow(in.at(0) - in.at(3), 2) / pow(2 * in.at(1), 2))));
                    return {{real,      chartBound.x, chartWidth},
                            {imaginary, chartBound.x, chartWidth}};
                }

                case ADD: {
                    complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
                    complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
                    complex<float> out0 = in0 + in1;
                    return {{out0.real(), chartBound.x, chartWidth},
                            {out0.imag(), chartBound.x, chartWidth}};
                }
                case SUBTRACT:
                    out = {in.at(0) - in.at(1), chartBound.x, chartWidth};
                    break;
                case MULTIPLY: {
                    complex<float> a = {x.at(REAL).at(0), x.at(IMAG).at(0)};
                    complex<float> b = {x.at(REAL).at(1), x.at(IMAG).at(1)};

                    auto result = a * b;
                    return {{result.real(), chartBound.x, chartWidth},
                            {result.imag(), chartBound.x, chartWidth}};
                }
                case DIVIDE: {
                    float a = x.at(REAL).at(1);
                    float b = x.at(IMAG).at(1);
                    float c = x.at(REAL).at(0);
                    float e = x.at(IMAG).at(0);

                    float denom = pow(a, 2) + pow(b, 2);

                    float r = (c * a + b * e) / denom;
                    float img = (a * d - c * e) / denom;
                    return {{r, chartBound.x, chartWidth},
                            {{img, chartBound.x, chartWidth}}};
                }
                case C:
                    x.at(REAL) = mConstantValueOutput;
                    x.at(IMAG).at(0) = 0;
                    return x;

                case CI:
                    x.at(REAL).at(0) = 0;
                    x.at(IMAG) = mConstantValueOutput;
                    return x;
                case X:
                    x.at(REAL).at(0) = x.at(REAL).at(0);
                    x.at(REAL).at(1) = chartBound.x;
                    x.at(REAL).at(2) = chartWidth;

                    x.at(IMAG).at(0) = 0;
                    x.at(IMAG).at(1) = chartBound.x;
                    x.at(IMAG).at(2) = chartWidth;
                    return x;
                case Y:
                    return {{x.at(IMAG).at(0),chartBound.x, chartWidth},
                            {0, chartBound.x, chartWidth}};
                case Z:
                    return {{x.at(REAL).at(0), chartBound.x, chartWidth},
                            {x.at(IMAG).at(0), chartBound.x, chartWidth}};
                case FILE:
                    return {{x.at(REAL).at(0), chartBound.x, chartWidth},
                            {0,                chartBound.x, chartWidth}};
                    break;
                case FFT: {
                    auto fft = computeFft(in.at(1), in.at(2), in.at(3));
                    return {{fft.first,  chartWidth, in.at(3)},
                            {fft.second, chartWidth, in.at(3)}};
                }
                case IFFT: {
                    auto fft = computeInverseFft(in.at(1), in.at(2), in.at(3));
                    return {{fft.first,  chartBound.x, chartWidth},
                            {fft.second, chartBound.x, chartWidth}};
                }
                case HARTLEY: {
                    auto fft = computeFft(in.at(1), in.at(2), in.at(3));
                    return {{sqrt(pow(fft.first, 2.0f) + pow(fft.second, 2.0f)), chartWidth, in.at(3)},
                            {0.0,                                                chartWidth, in.at(3)}};
                }
                case LAPLACE: {
                    mChart->setZBound(vec2(x.at(REAL).at(4), x.at(REAL).at(5)));
                    // Static resolution for now
                    auto laplace = computeLaplace(x.at(REAL).at(1), x.at(IMAG).at(1), in.at(2), in.at(3),
                                                  mChart->getMaxResolution());

                    x.at(REAL).at(0) = laplace.first;
                    return x;
                }
                case FIRST_DIFF: {
                    float diff = computeFirstDifference(in.at(0), in.at(1));
                    out = {diff, chartBound.x, chartWidth};
                    break;
                }
                case DOT:
                    return {{dot(vec2(x.at(0).at(0), x.at(1).at(0)),
                                 vec2(x.at(0).at(1), x.at(1).at(1))), chartBound.x, chartWidth}};
                case CROSS:
                    break;
                case CHART_2D: {
                    mChart->setResolution(100);

                    x.at(REAL).at(0) = in.at(0);
                    x.at(REAL).at(1) = in.at(1);
                    x.at(REAL).at(2) = chartBound.x;
                    x.at(REAL).at(3) = chartWidth;

                    x.at(IMAG).at(0) = 0.0;
                    x.at(IMAG).at(1) = 0.0;
                    x.at(IMAG).at(2) = 0.0;
                    x.at(IMAG).at(3) = 0.0;
                    return x;
                }

                case HEAT_MAP: {
                    mChart->setZBound(vec2(x.at(0).at(1), x.at(0).at(2)));
                    out = {x.at(0).at(0), chartBound.x, chartWidth};
                    break;
                }
                case COMBINE: {
                    return {{x.at(REAL).at(0)},
                            {x.at(REAL).at(1)}};
                }
                case SPLIT: {
                    return {{x.at(REAL).at(0), x.at(IMAG).at(0)},
                            {NAN, NAN}};
                }
                case NEURAL_CORE: {
                    if (mMlModel == nullptr) {
                        initializeNNModel();
                    }

                    float returnValue;
                    if (mMlModel->getTrainingInProgress()) {
                        vec2 thisChartBounds = mChart->getXBounds();
                        float span = thisChartBounds.y - thisChartBounds.x;
                        float inX = ((x.at(REAL).at(1) - thisChartBounds.x) / span) * mMlCache.size();
                        if (span > 0) {
                            int xIndex = 0;
                            if (inX >= 0 && (inX) < mMlCache.size() && !mMlCache.empty()) {
                                xIndex = (int) inX;
                                complex<float> y = mMlCache.at(xIndex);
                                returnValue = y.real();
                            }
                        }

                    } else {
                        mMlModel->setInput(x.at(REAL).at(1), 0);
                        mMlModel->compute();
                        returnValue = mMlModel->getOutputAt(0);
                    }

                    return {{returnValue, chartBound.x, chartWidth},
                            {x.at(REAL).at(0),      chartBound.x, chartWidth}};
                }
                case LAST:
                    break;
            }

            output.push_back(out);
        }

        return output;
    }

    array<array<SocketType, 8>, 2> COS_TYPE = {{{{VAR, CON, CON}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> POLY_TYPE = {{{{VAR, CON, CON, CON, CON}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> SQRT_TYPE = {{{{VAR}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> POW_TYPE = {{{{VAR, VAR}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> GAUSSIAN_TYPE = {{{{VAR, CON, CON}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> MORLET_TYPE = {{{{VAR, CON, CON, CON, CON}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> DIVIDE_TYPE = {{{{CON, CON}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> CI_TYPE = {{{{CON}}, {{CON}}}};
    array<array<SocketType, 8>, 2> Y_TYPE = {{{{VAR}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> Z_TYPE = {{{{VAR}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> FILE_TYPE = {{{{}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> IFFT_TYPE = {{{{VAR, VAR, CON, CON}}, {{VAR, VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> FFT_TYPE = {{{{VAR, VAR, CON, CON}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> HARTLEY_TYPE = {{{{VAR, VAR, CON, CON}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> LAPLACE_TYPE = {{{{VAR, VAR, CON, CON, CON, CON}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> FIRST_DIFF_TYPE = {{{{VAR, VAR}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> DOT_TYPE = {{{{CON, CON}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> CROSS_TYPE = {{{{VAR, VAR, VAR, VAR}}, {{VAR, VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> CHART_2D_TYPE = {{{{VAR, VAR, CON}}, {{VAR, VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> HEAT_MAP_TYPE = {{{{VAR, CON, CON}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> COMBINE_TYPE = {{{{VAR, VAR}}, {{VAR}}}};
    array<array<SocketType, 8>, 2> SPLIT_TYPE = {{{{VAR}}, {{VAR, VAR}}}};
    array<array<SocketType, 8>, 2> NEURAL_CORE_TYPE = {{{{VAR, VAR, CON, CON, CON, ENUM, ENUM}}, {{VAR, CON, CON}}}};
    array<array<SocketType, 8>, 2> NONE_TYPE = {{ {{NONE}}, {{NONE}} }};

    array<array<SocketType, 8>, 2> mSocketType = NONE_TYPE;

    array<array<SocketType, 8>, 2> getSocketType() {
        if (mSocketType.at(0).at(0) == NONE) {
            switch (mType) {
                case POLY: {
                    mSocketType = POLY_TYPE;
                    break;
                }
                case SIN:
                case COS: {
                    mSocketType = COS_TYPE;
                    break;
                }
                case TAN:
                case ABS:
                case EXP:
                case SQRT: {
                    mSocketType = SQRT_TYPE;
                    break;
                }
                case POW: {
                    mSocketType = POW_TYPE;
                    break;
                }
                case GAUSSIAN: {
                    mSocketType = GAUSSIAN_TYPE;
                    break;
                }
                case MORLET: {
                    mSocketType = MORLET_TYPE;
                    break;
                }
                case ADD:
                case SUBTRACT:
                case MULTIPLY:
                case DIVIDE: {
                    mSocketType = DIVIDE_TYPE;
                    break;
                }
                case C:
                case CI: {
                    mSocketType = CI_TYPE;
                    break;
                }
                case X:
                case Y: {
                    mSocketType = Y_TYPE;
                    break;
                }
                case Z: {
                    mSocketType = Z_TYPE;
                    break;
                }
                case FILE: {
                    mSocketType = FILE_TYPE;
                    break;
                }
                case IFFT: {
                    mSocketType = IFFT_TYPE;
                    break;
                }
                case FFT: {
                    mSocketType = FFT_TYPE;
                    break;
                }
                case HARTLEY: {
                    mSocketType = HARTLEY_TYPE;
                    break;
                }
                case LAPLACE: {
                    mSocketType = LAPLACE_TYPE;
                    break;
                }
                case FIRST_DIFF: {
                    mSocketType = FIRST_DIFF_TYPE;
                    break;
                }
                case DOT: {
                    mSocketType = DOT_TYPE;
                    break;
                }
                case CROSS: {
                    mSocketType = CROSS_TYPE;
                    break;
                }
                case CHART_2D: {
                    mSocketType = CHART_2D_TYPE;
                    break;
                }
                case HEAT_MAP: {
                    mSocketType = HEAT_MAP_TYPE;
                    break;
                }
                case COMBINE: {
                    mSocketType = COMBINE_TYPE;
                    break;
                }
                case SPLIT: {
                    mSocketType = SPLIT_TYPE;
                    break;
                }
                case NEURAL_CORE: {
                    mSocketType = NEURAL_CORE_TYPE;
                    break;
                }
                case LAST: {
                    mSocketType = NONE_TYPE;
                    break;
                }
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
            switch (mType) {
                case POLY:
                    mSocketCount = ivec2(5, 3);
                    break;
                case SIN:
                    mSocketCount = ivec2(3, 3);
                    break;
                case COS:
                    mSocketCount = ivec2(3, 3);
                    break;
                case TAN:
                    mSocketCount = ivec2(1, 3);
                    break;
                case ABS:
                    mSocketCount = ivec2(1, 3);
                    break;
                case EXP:
                    mSocketCount = ivec2(1, 3);
                    break;
                case SQRT:
                    mSocketCount = ivec2(1, 3);
                    break;
                case POW:
                    mSocketCount = ivec2(2, 3);
                    break;
                case GAUSSIAN:
                    mSocketCount = ivec2(3, 3);
                    break;
                case MORLET:
                    mSocketCount = ivec2(5, 3);
                    break;
                case ADD:
                    mSocketCount = ivec2(2, 3);
                    break;
                case SUBTRACT:
                    mSocketCount = ivec2(2, 3);
                    break;
                case MULTIPLY:
                    mSocketCount = ivec2(2, 3);
                    break;
                case DIVIDE:
                    mSocketCount = ivec2(2, 3);
                    break;
                case C:
                    mSocketCount = ivec2(0, 1);
                    break;
                case CI:
                    mSocketCount = ivec2(0, 1);
                    break;
                case X:
                    mSocketCount = ivec2(0, 3);
                    break;
                case Y:
                    mSocketCount = ivec2(0, 3);
                    break;
                case Z:
                    mSocketCount = ivec2(0, 3);
                    break;
                case FILE:
                    mSocketCount = ivec2(0, 1);
                    break;
                case FFT:
                    mSocketCount = ivec2(4, 3);
                    break;
                case IFFT:
                    mSocketCount = ivec2(4, 3);
                    break;
                case HARTLEY:
                    mSocketCount = ivec2(4, 3);
                    break;
                case LAPLACE:
                    mSocketCount = ivec2(6, 3);
                    break;
                case FIRST_DIFF:
                    mSocketCount = ivec2(2, 3);
                    break;
                case DOT:
                    mSocketCount = ivec2(2, 3);
                    break;
                case CROSS:
                    mSocketCount = ivec2(4, 4);
                    break;
                case CHART_2D:
                    mSocketCount = ivec2(3, 4);
                    break;
                case HEAT_MAP:
                    mSocketCount = ivec2(3, 3);
                    break;
                case COMBINE:
                    mSocketCount = ivec2(2, 1);
                    break;
                case SPLIT:
                    mSocketCount = ivec2(1, 2);
                    break;
                case NEURAL_CORE:
                    mSocketCount = ivec2(7, 3);
                    break;
                case LAST:
                    mSocketCount = ivec2(0, 0);
                    break;
            }
        }

        return mSocketCount;
    }

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
            case NEURAL_CORE:
                return {0.0, 0.0, 0.05, -5, -1, 0, 0};
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
            case NEURAL_CORE:
                return {"Training Data", "X", "Step Size", "Window Start", "Window Width", "Optimizer", "Activation"};
            default:
                return {" ", " ", " ", " ", " ", " "};
        }
    }

    vector<string> getButtonNames() {
        switch (mType) {
            case NEURAL_CORE:
                return {"Train", "Reset"};
            default:
                return {};
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
                return "pow";
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
            case FILE:
                return "file";
            case IFFT:
                return "ifft";
            case FFT:
                return "fft";
            case HARTLEY:
                return "hartley";
            case LAPLACE:
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
                return vec2(400, 400);
            case NEURAL_CORE:
                return vec2(300, 100);
            default:
                return vec2(80, 70);
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

    function<void(ZButton *sender)> getButtonCallback(int index) {
        switch (mType) {
            case NEURAL_CORE: {
                switch (index) {
                    case 0: {
                        return [this](ZButton *sender) {
                            // Train the network

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
                                    mMlModel->setActivationFunction(Neuron::Activation::TANH);
                                    break;
                                }
                                case 1: {
                                    mMlModel->setActivationFunction(Neuron::Activation::SIGMOID);
                                    break;
                                }
                                case 2: {
                                    mMlModel->setActivationFunction(Neuron::Activation::RELU);
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
                                sender->setText("Train");
                            } else {
                                mMlModel->trainNetworkAsync(10000);
                                sender->setText("Stop");
                            }
                        };
                    }
                    case 1: {
                        return [this](ZButton *sender) {
                            mMlModel->resetNetwork();
                        };
                    }
                    default:
                        return nullptr;

                }

            }
            default:
                return nullptr;
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

    static vec4 getSocketColor(SocketType type) {
        const vec4 mVariableColor = vec4(1, 0.611956, 0.052950, 1);
        const vec4 mConstantColor = vec4(1, 0.437324, 0.419652, 1);
        const vec4 mEnumColor = vec4(0.038825, 0.538225, 0.048049, 1.000000);
        const vec4 mNNColor = vec4(0.023195, 0.223442, 0.538225, 1.000000);

        switch (type) {
            case CON:
                return mConstantColor;
            case VAR:
                return mVariableColor;
            case ENUM:
                return mEnumColor;
            case NN:
                return mNNColor;

        }
    }

    static vec4 getNodeColor(Type type) {

        switch (type) {
            case C:
            case CI:
                return getSocketColor(CON);
            case X:
            case Y:
            case Z:
                return getSocketColor(VAR);
            case NEURAL_CORE:
                return getSocketColor(NN);
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
            case HEAT_MAP:
                return IMAGE;
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

    float sumInputs(vec2 input, int socketIndex) {
        float sum = 0.0;
        auto inputSocket = mInputIndices.at(socketIndex);

        vector<float> x = vector<float>(MAX_INPUT_COUNT, input.x);
        vector<float> y = vector<float>(MAX_INPUT_COUNT, input.y);

        for (auto singleInput : inputSocket) {
            sum += singleInput.first->evaluate({x, y}).at(
                    0).at(singleInput.second);
        }

        return sum;
    }

    float sumInputs(float x, int socketIndex, int var) {
        float summedInputs = 0.0;
        auto inputSocket = mInputIndices.at(socketIndex);
        for (auto singleInput : inputSocket) {
            summedInputs += singleInput.first->evaluate(vector<vector<float>>(2, vector<float>(MAX_INPUT_COUNT, x))).at(
                    var).at(singleInput.second);
        }

        if (inputSocket.empty()) {
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

    vector<vector<float>> evaluate(vector<vector<float>> x);

    vector<vector<float>> evaluate(vector<vector<float>> x, ZNodeView *root);

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

private:
    bool mInvalid = true;

    vector<ZView *> mSocketsIn;
    vector<ZView *> mSocketsOut;

    vector<ZLabel *> mSocketInLabels;
    vector<ZLabel *> mSockOutLabels;

    Type mType = ADD;

    ZLabel *mOutputLabel;
    ZLabel *mNameLabel;

    ZLabel *mXMinLabel;
    ZLabel *mXMaxLabel;
    ZLabel *mYMinLabel;
    ZLabel *mYMaxLabel;

    /**
     * Number of line segments on the chart
     */

    // Todo: remove  these
    ZLineChart *mChart;
    vector<vector<float>> mPointCache;
    vector<float> mPointCache1D;

    vector<float> mConstantValueOutput = vector<float>(MAX_OUTPUT_COUNT, 0.0);
    vector<float> mConstantValueInput = vector<float>(MAX_INPUT_COUNT, 0.0);

    // Center magnitude is at index 6
    vector<int> mConstantMagnitudeOutput = vector<int>(MAX_OUTPUT_COUNT, DEFAULT_MAGNITUDE);
    vector<int> mConstantMagnitudeInput = vector<int>(MAX_INPUT_COUNT, DEFAULT_MAGNITUDE);

    vector<int> mMagPrecision = {8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0};

    function<void(ZView *sender, ZNodeView *node, int socketIndex,
                  bool isInput, float initialValue, string name)> mShowMagnitudeView;

    function<void(ZView *sender, ZNodeView *node, int socketIndex,
                  bool isInput, float initialValue, string name, vector<string> enumNames)> mShowEnumView;

    function<void(ZNodeView *node)> mInvalidateListener;

    vector<complex<float>> mFftCache;
    vector<complex<float>> mMlCache;

    vector<vector<float>> mLaplaceCache;

    void onMouseEvent(int button, int action, int mods, int sx, int sy) override;

    void clearInvalidateNode();

    bool isInvalid();

    void updateChart1D();

    void updateChart2D();

    void updateLabelVisibility();

    void updateChart1D2X();

    void setOutputLabel(float output) const;

    void updateChartHeatMap();

    void onCursorPosChange(double x, double y);

    void hideSocketLabels();

    void init();

};


#endif //ZPATH_NODEVIEW_H
