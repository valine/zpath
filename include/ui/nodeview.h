//
// Created by lukas on 10/4/20.
//

#ifndef ZPATH_NODEVIEW_H
#define ZPATH_NODEVIEW_H

static const int SOCKET_SIZE = 15;
static const int MAX_INPUT_COUNT = 6;
static const int MAX_OUTPUT_COUNT = 4;
static const int CHART_RES_THRESHOLD = 4;
static const int CHART_SIDE_MARGIN_WIDE = 20;
static const int MIN_MARGIN = 3;
static const int CHART_TOP_MARGIN = 15;
static const int DEFAULT_MAGNITUDE = 6;
static const int LABEL_THRESHOLD = 120;
static const int LABEL_THRESHOLD_X = 160;
using namespace std;
#include <vector>
#include "zview.h"
#include "zchart.h"
#include "zlinechart.h"
#include <complex.h>
#include <utils/zfft.h>

class ZNodeView : public ZView {
public:

    ZNodeView(float maxWidth, float maxHeight, ZView *parent);
    void setShowMagPickerListener(
            function<void(ZView *sender, ZNodeView *node, int socketIndex,
                    bool isInput, float initialValue, string name)> onValueSelect);
    void setInvalidateListener(function<void(ZNodeView* node)> listener);

    enum Type {
        SIN,
        COS,
        TAN,
        EXP,
        SQRT,
        POW,
        GAUSSIAN,
        MORLET,
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        C, // Constant value
        X,
        Y,
        FILE,
        FFT,
        IFFT,
        HARTLEY,
        LAPLACE,
        FIRST_DIFF,
        SECOND_DIFF,
        DOT,
        CROSS,
        CHART_2D,
        LAST // Fake enum to allow easy iteration
    };

    enum SocketType {
        CON,
        VAR
    };

    enum ChartResMode {
        ADAPTIVE,
        STATIC
    };

    enum ChartType {
        LINE_1D,
        LINE_2D,
        LINE_1D_2X

    };

    pair<float, float> computeFft(float in, float start, float width) {
        int res = std::max((int) mChart->getXBounds().y, 5);
        mChart->setResolution(res);
        if (mFftCache.empty()) {
            for (int i = 0; i < res * 2; i++) {
                float summedInput = 0.0;
                float x = mix(start, start + width, (float) i / (float) (res * 2));
                auto inputSocket = mInputIndices.at(0);
                for (auto singleInput : inputSocket) {
                    summedInput += singleInput.first->evaluate(vector<vector<float>>(MAX_INPUT_COUNT, {x})).at(
                            singleInput.second).at(0);
                }
                mFftCache.emplace_back(summedInput, 0.0f);
            }
            ZFFt::transform(mFftCache);
        }

        vec2 thisChartBounds = mChart->getXBounds();
        float span = thisChartBounds.y - thisChartBounds.x;

        pair<float, float> returnValue = {NAN, NAN};
        if (span > 0) {
            int xIndex = 0;
            if (in >= 0 && (in) < mFftCache.size() && !mFftCache.empty()) {
                xIndex = (int) round(in);
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
                float summedInput = 0.0;
                float summedInput2 = 0.0;
                auto inputSocket = mInputIndices.at(0);
                auto inputSocket2 = mInputIndices.at(1);
                for (auto singleInput : inputSocket) {
                    summedInput += singleInput.first->evaluate(vector<vector<float>>(MAX_INPUT_COUNT, {(float) i})).at(
                            singleInput.second).at(0);
                }

                for (auto singleInput : inputSocket2) {
                    summedInput2 += singleInput.first->evaluate(vector<vector<float>>(MAX_INPUT_COUNT, {(float) i})).at(
                            singleInput.second).at(0);
                }
                mFftCache.emplace_back(summedInput, summedInput2);
            }
            ZFFt::inverseTransform(mFftCache);
        }

        pair<float, float> returnValue = {NAN, NAN};
        int xIndex =  (int) ((in * (mFftCache.size() - 1)) / windowSize);
        if (xIndex >= 0 && !mFftCache.empty() && windowSize > 0 && xIndex < mFftCache.size()) {

            complex<float> y = mFftCache.at(xIndex);
            returnValue = {y.real(), y.imag()};
        }

        mChart->invalidate();
        return returnValue;
    }

    vector<vector<float>> evaluate(vector<vector<float>> x);
    vector<vector<float>> evaluate(vector<vector<float>> x, ZNodeView* root);

    vector<vector<float>> compute(const vector<vector<float>>& x, Type type) {
        vec2 chartBound = mChart->getXBounds();
        float chartWidth = chartBound.y - chartBound.x;

        vector<float> in;
        for (vector<float> firstInput : x) {
            in.push_back(firstInput.at(0));
        }

        switch (type) {
            case SIN:
                return {{sin(in.at(0) * in.at(1))}, {chartBound.x}, {chartWidth}};
            case COS:return {{cos(in.at(0) * in.at(1))}, {chartBound.x}, {chartWidth}};
            case TAN:return {{tan(in.at(0))}, {chartBound.x}, {chartWidth}};
            case EXP:return {{exp(in.at(0))}, {chartBound.x}, {chartWidth}};
            case SQRT:return {{sqrt(in.at(0))}, {chartBound.x}, {chartWidth}};
            case POW:return {{pow(in.at(0), in.at(1))}, {chartBound.x}, {chartWidth}};
            case GAUSSIAN:return {{(float) (in.at(2) * exp(-pow(in.at(0), 2) / pow(2 * in.at(1), 2)))},
                                  {chartBound.x}, {chartWidth}};
            case MORLET:return {{(float) (
                        cos(in.at(0) * in.at(4)) * // sinusoid
                        (in.at(2) * exp(-pow(in.at(0) - in.at(3), 2) / pow(2 * in.at(1), 2))))}, // gaussian
                        {chartBound.x}, {chartWidth}}; // Chart
            case ADD:return {{in.at(0) + in.at(1)}, {chartBound.x}, {chartWidth}};
            case SUBTRACT:return {{in.at(0) - in.at(1)}, {chartBound.x}, {chartWidth}};
            case MULTIPLY:return {{in.at(0) * in.at(1)}, {chartBound.x}, {chartWidth}};
            case DIVIDE:return {{in.at(0) / in.at(1)}, {chartBound.x}, {chartWidth}};
            case C:return {mConstantValueOutput};
            case X:return {{in.at(0)}, {chartBound.x}, {chartWidth}};
            case Y:return {{x.at(0).at(1)}, {chartBound.x}, {chartWidth}};
            case FILE:break;
            case FFT: {
                auto fft = computeFft(in.at(1), in.at(2), in.at(3));
                return {{fft.first}, {fft.second}, {chartWidth}, {in.at(3)}};
            }
            case IFFT: {
                auto fft = computeInverseFft(in.at(2), in.at(3), in.at(4));
                return {{fft.first}, {fft.second}, {chartBound.x}, {chartWidth}};
            }
            case HARTLEY: {
                auto fft = computeFft(in.at(1), in.at(2), in.at(3));
                return {{sqrt(pow(fft.first, 2.0f) + pow(fft.second, 2.0f))}, {chartBound.x}, {chartWidth}};
            }
            case LAPLACE:break;
            case FIRST_DIFF:break;
            case SECOND_DIFF:break;
            case DOT:break;
            case CROSS:break;
            case CHART_2D: {
                mChart->setResolution(((int) in.at(2)));
                return {{in.at(0)}, {in.at(1)}, {chartBound.x}, {chartWidth}};
            }
            case LAST:break;
        }

        return vector<vector<float>>(3);
    }

    /**
     * First value is input count, second value is output count. If input count is zero node
     * is an input node like a constant or number range. If output count is zero the node is
     * an output node like a plot or label.
     * @return Vector with node socket input and output count
     */
    ivec2 getSocketCount() {
        switch (mType) {
            case SIN:return ivec2(2,3);
            case COS:return ivec2(2,3);
            case TAN:return ivec2(1,3);
            case EXP:return ivec2(1,3);
            case SQRT:return ivec2(1,3);
            case POW:return ivec2(2,3);
            case GAUSSIAN:return ivec2(3,3);
            case MORLET:return ivec2(5,3);
            case ADD:return ivec2(2,3);
            case SUBTRACT:return ivec2(2,3);
            case MULTIPLY:return ivec2(2,3);
            case DIVIDE:return ivec2(2,3);
            case C:return ivec2(0,1);
            case X:return ivec2(0,3);
            case Y:return ivec2(0,3);
            case FILE:return ivec2(0,1);
            case FFT:return ivec2(4,4);
            case IFFT:return ivec2(5,4);
            case HARTLEY:return ivec2(4,3);
            case LAPLACE:return ivec2(3,3);
            case FIRST_DIFF:return ivec2(3,3);
            case SECOND_DIFF:return ivec2(3,3);
            case DOT:return ivec2(4,3);
            case CROSS:return ivec2(4,4);
            case CHART_2D: return ivec2(3,4);
            case LAST:return ivec2(0,0);
        }
    }

    pair<vector<SocketType>,vector<SocketType>>  getSocketType() {
        switch (mType) {
            case SIN:
            case COS:
                return {{VAR, CON}, {VAR, CON, CON}};
            case TAN:
            case EXP:
            case SQRT:return {{VAR}, {VAR, CON, CON}};
            case POW:return {{VAR, VAR}, {VAR, CON, CON}};
            case GAUSSIAN:return {{VAR, CON, CON}, {VAR, CON, CON}};
            case MORLET:return {{VAR, CON, CON, CON, CON}, {VAR, CON, CON}};
            case ADD:
            case SUBTRACT:
            case MULTIPLY:
            case DIVIDE:return {{VAR, VAR}, {VAR, CON, CON}};
            case C:return {{}, {CON}};
            case X:
            case Y:return {{}, {VAR, CON, CON}};
            case FILE:return {{}, {VAR, CON, CON}};
            case IFFT:return {{VAR, VAR, VAR, CON, CON}, {VAR, VAR, CON, CON}};
            case FFT:return {{VAR, VAR, CON, CON}, {VAR, VAR, CON, CON}};
            case HARTLEY:return {{VAR, VAR, CON, CON}, {VAR, CON, CON}};
            case LAPLACE:return {{VAR, CON, CON}, {VAR, CON, CON}};
            case FIRST_DIFF:return {{VAR, CON, CON}, {VAR, CON, CON}};
            case SECOND_DIFF:return {{VAR, CON, CON}, {VAR, CON, CON}};
            case DOT:return {{VAR, VAR, VAR, VAR}, {VAR, CON, CON}};
            case CROSS:return  {{VAR, VAR, VAR, VAR}, {VAR, VAR, CON, CON}};
            case CHART_2D: return {{VAR, VAR, CON}, {VAR, VAR, CON, CON}};
            case LAST:return {};
        }
    }

    static string getName(Type type) {
        switch (type) {
            case SIN:return "sin";
            case COS:return "cos";
            case TAN:return "tan";
            case EXP:return "exp";
            case SQRT:return "sqrt";
            case POW:return "pow";
            case GAUSSIAN:return "gaussian";
            case MORLET:return "wavelet";
            case ADD:return "+";
            case SUBTRACT:return "-";
            case MULTIPLY:return "*";
            case DIVIDE:return "/";
            case C:return "C";
            case X:return "x";
            case Y:return "y";
            case FILE:return "file";
            case IFFT:return "Inverse FFT";
            case FFT:return "FFT";
            case HARTLEY:return "Hartley";
            case LAPLACE:return "Laplace";
            case FIRST_DIFF:return "1st diff";
            case SECOND_DIFF:return "2nd diff";
            case DOT:return "dot";
            case CROSS:return "cross";
            case CHART_2D:return "2D Chart";
            case LAST:return "none";
        }
    }

    static vec2 getNodeSize(Type type) {
        switch(type) {
            case C:
                return vec2(80, 20);
            case CHART_2D:
            case FFT:
            case HARTLEY:
            case IFFT:
                return vec2(200, 200);

            default:
                return vec2(80, 100);
        }
    }

    static vector<float> getDefaultInput(Type type) {
        switch(type) {
            case SIN:
            case COS:
                return {0.0, 1.0};
            case GAUSSIAN:
                // X, width, height
                return {0.0, 1.0, 1.0};
            case MORLET:
                return {0.0, 1.0, 1.0, 0.0, 1.0};
            case IFFT:return {0.0, 0.0,0.0, 1.0, 1.0};
            case FFT:
            case HARTLEY:
                return {0.0, 0.0, 0.0, 1.0};
            case CHART_2D:
                return {0.0,0.0,100};
            default:
                return vector<float>(MAX_INPUT_COUNT, 0.0);
        }
    }


    static bool isOutputLabelVisible(Type type) {
        switch(type) {
            case C:
                return true;
            default:
                return false;
        }
    }

    vector<string> getSocketName() {
        switch (mType) {
            case SIN:
            case COS:return {"", "Frequency"};
            case GAUSSIAN: return {"", "Width", "Height"};
            case MORLET: return {"", "Width", "Height", "Offset", "Frequency"};
            case IFFT:return {"", "","", "Window Start", "Window Size"};
            case HARTLEY:
            case FFT: return {"", "", "Window Start", "Window Size"};
            case CHART_2D: return {"", "", "Resolution"};
            default: vector<string>(MAX_INPUT_COUNT, "");
        }
    }

    vec4 getNodeColor(Type type) {
        switch (type) {
            case C:
                return mConstantColor;
            case X:
            case Y:
                return mVariableColor;
            default:
                return vec4(1);
        }
    }

    static ChartResMode getChartResolutionMode(Type type) {
        switch (type) {
            default: return ADAPTIVE;
            case IFFT: return ADAPTIVE;
            case CHART_2D:
            case FFT:
            case HARTLEY:return STATIC;
        }
    }

    static ChartType getChartType(Type type) {
        switch (type) {
            default: return LINE_1D;
            case HARTLEY: return LINE_1D;
            case FFT:
            case IFFT:return LINE_1D_2X;
            case CHART_2D: return LINE_2D;
        }
    }

    void setType(Type type);

    Type getType() {
        return mType;
    }

    vector<vector<pair<ZNodeView*, int>>> mInputIndices;
    vector<vector<pair<ZNodeView*, int>>> mOutputIndices;

    vector<ZView*> getSocketsIn();
    vector<ZView*> getSocketsOut();

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

    void setMaxWidth(int width) override;
private:
    bool mInvalid = true;

    vector<ZView*> mSocketsIn;
    vector<ZView*> mSocketsOut;

    Type mType = ADD;

    ZLabel* mOutputLabel;
    ZLabel* mNameLabel;

    ZLabel* mXMinLabel;
    ZLabel* mXMaxLabel;
    ZLabel* mYMinLabel;
    ZLabel* mYMaxLabel;

    /**
     * Number of line segments on the chart
     */

    // Todo: remove  these
    ZLineChart* mChart;
    vector<vector<float>> mPointCache;

    vector<float> mConstantValueOutput = vector<float>(MAX_OUTPUT_COUNT, 0.0);
    vector<float> mConstantValueInput = vector<float>(MAX_INPUT_COUNT, 0.0);

    // Center magnitude is at index 6
    vector<int> mConstantMagnitudeOutput = vector<int>(MAX_OUTPUT_COUNT, DEFAULT_MAGNITUDE);
    vector<int> mConstantMagnitudeInput = vector<int>(MAX_INPUT_COUNT, DEFAULT_MAGNITUDE);

    vec4 mVariableColor = vec4(1, 0.611956, 0.052950, 1);
    vec4 mConstantColor = vec4(1, 0.437324, 0.419652, 1);

    function<void(ZView* sender, ZNodeView* node, int socketIndex,
            bool isInput, float initialValue, string name)> mShowMagnitudeView;
    function<void(ZNodeView* node)> mInvalidateListener;

    vector<complex<float>> mFftCache;

    bool onMouseEvent(int button, int action, int mods, int sx, int sy) override;

    void clearInvalidateNode();
    bool isInvalid();

    void updateChart1D();
    void updateChart2D();

    void updateLabelVisibility();

    void updateChart1D2X();
};






#endif //ZPATH_NODEVIEW_H
