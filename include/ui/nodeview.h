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
        LINE_1D_2X,
        LINE_2D,
        IMAGE
    };

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
        DOT,
        CROSS,
        CHART_2D,
        COMBINE,
        SPLIT,
        HEAT_MAP,
        LAST // Fake enum to allow easy iteration
    };

    vector<vector<float>> compute(const vector<vector<float>>& x, Type type) {
        vec2 chartBound = mChart->getXBounds();
        float chartWidth = chartBound.y - chartBound.x;
        vector<vector<float>> output;
        for (int d = 0; d < x.size(); d++) {
            vector<float> in = x.at(d);
            vector<float> out;
            switch (type) {
                case SIN:
                    out = {in.at(2) * sin(in.at(0) * in.at(1)), chartBound.x, chartWidth};
                    break;
                case COS:
                    out = {in.at(2) * cos(in.at(0) * in.at(1)), chartBound.x, chartWidth};
                    break;
                case TAN:
                    out = {tan(in.at(0)), chartBound.x, chartWidth};
                    break;
                case EXP:
                    out = {exp(in.at(0)), chartBound.x, chartWidth};
                    break;
                case SQRT:
                    out = {sqrt(in.at(0)), chartBound.x, chartWidth};
                    break;
                case POW:
                    out = {pow(in.at(0), in.at(1)), chartBound.x, chartWidth};
                    break;
                case GAUSSIAN:
                    out = {(float) (in.at(2) * exp(-pow(in.at(0), 2) / pow(2 * in.at(1), 2))),
                           chartBound.x, chartWidth};
                    break;
                case MORLET:
                    out = {(float) (
                            cos(in.at(0) * in.at(4)) * // sinusoid
                            (in.at(2) * exp(-pow(in.at(0) - in.at(3), 2) / pow(2 * in.at(1), 2)))), // gaussian
                           chartBound.x, chartWidth}; // Chart
                    break;
                case ADD:
                    out = {in.at(0) + in.at(1), chartBound.x, chartWidth};
                    break;
                case SUBTRACT:
                    out = {in.at(0) - in.at(1), chartBound.x, chartWidth};
                    break;
                case MULTIPLY:
                    out = {in.at(0) * in.at(1), chartBound.x, chartWidth};
                    break;
                case DIVIDE:
                    out = {in.at(0) / in.at(1), chartBound.x, chartWidth};
                    break;
                case C:
                    out = mConstantValueOutput;
                    break;
                case X:
                    return {{x.at(0).at(0), chartBound.x, chartWidth},
                            {NAN,      chartBound.x, chartWidth}};
                case Y:
                    return {{NAN,      chartBound.x, chartWidth},
                            {x.at(1).at(0), chartBound.x, chartWidth}};
                case FILE:
                    break;
                case FFT: {
                    auto fft = computeFft(in.at(1), in.at(2), in.at(3));
                    return  {{fft.first, chartWidth, in.at(3)}, {fft.second, chartWidth, in.at(3)}};
                }
                case IFFT: {
                    auto fft = computeInverseFft(in.at(1), in.at(2), in.at(3));
                    return  {{fft.first, chartBound.x, chartWidth}, {fft.second, chartBound.x, chartWidth}};
                }
                case HARTLEY: {
                    auto fft = computeFft(in.at(1), in.at(2), in.at(3));
                    out = {sqrt(pow(fft.first, 2.0f) + pow(fft.second, 2.0f)), chartBound.x, chartWidth};
                    break;
                }
                case LAPLACE: {
                    // Static resolution for now
                    auto laplace = computeLaplace(x.at(0).at(1), x.at(1).at(1), in.at(2), in.at(3), 10);
                    return {{laplace.first}};
                }
                case FIRST_DIFF: {
                    float diff = computeFirstDifference(in.at(0), in.at(1));
                    out = {diff, chartBound.x, chartWidth};
                    break;
                }
                case DOT:
                    return {{dot(vec2(x.at(0).at(0), x.at(1).at(0)), vec2(x.at(0).at(1), x.at(1).at(1))), chartBound.x, chartWidth}};
                case CROSS:
                    break;
                case CHART_2D: {
                    mChart->setResolution(((int) in.at(2)));
                    out = {in.at(0), in.at(1), chartBound.x, chartWidth};
                    break;
                }

                case HEAT_MAP: {
                    mChart->setZBound(vec2(x.at(0).at(1), x.at(0).at(2)));
                    out = {x.at(0).at(0), chartBound.x, chartWidth};
                    break;
                }
                case COMBINE: {
                    return {{x.at(0).at(0)},
                            {x.at(1).at(1)}};

                }
                case SPLIT: {
                    return {{x.at(0).at(0), x.at(1).at(0)}, {NAN, NAN}};

                }
                case LAST:
                    break;
            }

            output.push_back(out);
        }

        return output;
    }

    /**
     * First value is input count, second value is output count. If input count is zero node
     * is an input node like a constant or number range. If output count is zero the node is
     * an output node like a plot or label.
     * @return Vector with node socket input and output count
     */
    ivec2 getSocketCount() {
        switch (mType) {
            case SIN:return ivec2(3,3);
            case COS:return ivec2(3,3);
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
            case FFT:return ivec2(4,3);
            case IFFT:return ivec2(4,3);
            case HARTLEY:return ivec2(4,3);
            case LAPLACE:return ivec2(4,3);
            case FIRST_DIFF:return ivec2(2,3);
            case DOT:return ivec2(2,3);
            case CROSS:return ivec2(4,4);
            case CHART_2D: return ivec2(3,4);
            case HEAT_MAP: return ivec2(3,3);
            case COMBINE: return ivec2(2,1);
            case SPLIT: return ivec2(1,2);
            case LAST:return ivec2(0,0);
        }
    }

    pair<vector<SocketType>,vector<SocketType>>  getSocketType() {
        switch (mType) {
            case SIN:
            case COS:
                return {{VAR, CON, CON}, {VAR, CON, CON}};
            case TAN:
            case EXP:
            case SQRT:return {{VAR}, {VAR, CON, CON}};
            case POW:return {{VAR, VAR}, {VAR, CON, CON}};
            case GAUSSIAN:return {{VAR, CON, CON}, {VAR, CON, CON}};
            case MORLET:return {{VAR, CON, CON, CON, CON}, {VAR, CON, CON}};
            case ADD:
            case SUBTRACT:
            case MULTIPLY:
            case DIVIDE:return {{CON, CON}, {VAR, CON, CON}};
            case C:return {{}, {CON}};
            case X:
            case Y:return {{}, {VAR, CON, CON}};
            case FILE:return {{}, {VAR, CON, CON}};
            case IFFT:return {{VAR, VAR, CON, CON}, {VAR, VAR, CON, CON}};
            case FFT:return {{VAR, VAR, CON, CON}, {VAR, CON, CON}};
            case HARTLEY:return {{VAR, VAR, CON, CON}, {VAR, CON, CON}};
            case LAPLACE:return {{VAR, VAR, CON, CON}, {VAR, CON, CON}};
            case FIRST_DIFF:return {{VAR, VAR}, {VAR, CON, CON}};
            case DOT:return {{CON, CON}, {VAR, CON, CON}};
            case CROSS:return  {{VAR, VAR, VAR, VAR}, {VAR, VAR, CON, CON}};
            case CHART_2D: return {{VAR, VAR, CON}, {VAR, VAR, CON, CON}};
            case HEAT_MAP: return {{VAR, CON, CON}, {VAR, CON, CON}};
            case COMBINE: return {{VAR, VAR}, {VAR}};
            case SPLIT: return {{VAR}, {VAR, VAR}};
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
            case DOT:return "dot";
            case CROSS:return "cross";
            case CHART_2D:return "2D Chart";
            case HEAT_MAP:return "Heat Map";
            case COMBINE:return "Combine";
            case SPLIT:return "Split";
            case LAST:return "none";
        }
    }

    static vec2 getNodeSize(Type type) {
        switch(type) {
            case C:
                return vec2(80, 20);
            case CHART_2D:
            case HEAT_MAP:
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
            case ADD:
            case SUBTRACT: return {0.0, 0.0};
            case MULTIPLY:
            case DIVIDE: return {1.0, 1.0};
            case SIN:
            case COS:
                return {0.0, 1.0, 1.0};
            case GAUSSIAN:
                // X, width, height
                return {0.0, 1.0, 1.0};
            case MORLET:
                return {0.0, 1.0, 1.0, 0.0, 1.0};
            case IFFT:return {0.0,0.0, 1.0, 1.0};
            case FFT:
            case HARTLEY:
                return {0.0, 0.0, 0.0, 1.0};
            case LAPLACE:
                return {0.0, 0.0, 0.0, 1.0};
            case CHART_2D:
                return {0.0,0.0,100};
            case DOT:
                return {1.0, 1.0};
            case HEAT_MAP:
                return {0.0, -1.0, 1.0};
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
            case ADD:
            case SUBTRACT:
            case MULTIPLY:
            case DIVIDE: return {"", ""};
            case SIN:
            case COS:return {"", "Frequency", "Height"};
            case GAUSSIAN: return {"", "Width", "Height"};
            case MORLET: return {"", "Width", "Height", "Offset", "Frequency"};
            case IFFT:return {"", "", "Window Start", "Window Size"};
            case HARTLEY:
            case FFT: return {"", "", "Window Start", "Window Size"};
            case LAPLACE: return {"", "", "Window Start", "Window Size"};
            case CHART_2D: return {"", "", "Resolution"};
            case HEAT_MAP: return {"", "Z Min", "Z Max"};
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
            default: return LINE_1D_2X;
            case HARTLEY: return LINE_1D;
            case FFT:
            case IFFT:return LINE_1D_2X;
            case CHART_2D: return LINE_2D;
            case LAPLACE:
            case HEAT_MAP: return IMAGE;
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
        int xi = (int) std::min(std::max(0.0f, x), (float) resolution - 1.0f);
        int yi = (int) std::min(std::max(0.0f, y), (float) resolution - 1.0f);
        if (!mLaplaceCache.empty()) {
            return {mLaplaceCache.at(xi).at(yi), 0};
        }

        auto fres = (float) resolution;
        int socketIndex = 0;
        int dimenIndex = 0;

        vector<float> timeDomain;
        for (int i = 0; i < resolution; i++) {
            float t = ((i / fres) * windowSize) + start;
            float summedInput = sumInputs(t, socketIndex, dimenIndex);
            timeDomain.push_back(summedInput);
        }

        mLaplaceCache = vector<vector<float>>(resolution, vector<float>(resolution, 0));
        for (int fi = 0; fi < resolution; fi++) {
            for (int ei = 0; ei < resolution; ei++) {

                float freq = ((fi / fres) * windowSize) + start;
                float expo = ((ei / fres) * windowSize) + start;
                float output = 0;
                for (int t = 0; t < resolution; t++) {
                    output += (timeDomain.at(t) * cos(((freq * t * M_PI * 2)))) * exp((expo * t * M_PI * 2));
                }

                mLaplaceCache.at(fi).at(ei) = output;
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

    float sumInputs(float x, int socketIndex, int var) const {
        float summedInput = 0.0;
        auto inputSocket = mInputIndices.at(socketIndex);
        for (auto singleInput : inputSocket) {
            summedInput += singleInput.first->evaluate(vector<vector<float>>(2, vector<float>(MAX_INPUT_COUNT, x))).at(
                    var).at(singleInput.second);
        }
        return summedInput;
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

    void copyParameters(ZNodeView *node);
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
    vector<float> mPointCache1D;

    vector<float> mConstantValueOutput = vector<float>(MAX_OUTPUT_COUNT, 0.0);
    vector<float> mConstantValueInput = vector<float>(MAX_INPUT_COUNT, 0.0);

    // Center magnitude is at index 6
    vector<int> mConstantMagnitudeOutput = vector<int>(MAX_OUTPUT_COUNT, DEFAULT_MAGNITUDE);
    vector<int> mConstantMagnitudeInput = vector<int>(MAX_INPUT_COUNT, DEFAULT_MAGNITUDE);

    vec4 mVariableColor = vec4(1, 0.611956, 0.052950, 1);
    vec4 mConstantColor = vec4(1, 0.437324, 0.419652, 1);
    vector<int> mMagPrecision = {8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0};

    function<void(ZView* sender, ZNodeView* node, int socketIndex,
            bool isInput, float initialValue, string name)> mShowMagnitudeView;
    function<void(ZNodeView* node)> mInvalidateListener;

    vector<complex<float>> mFftCache;
    vector<vector<float>> mLaplaceCache;

    bool onMouseEvent(int button, int action, int mods, int sx, int sy) override;

    void clearInvalidateNode();
    bool isInvalid();

    void updateChart1D();
    void updateChart2D();

    void updateLabelVisibility();

    void updateChart1D2X();

    void setOutputLabel(float output) const;

    void updateChartHeatMap();
};






#endif //ZPATH_NODEVIEW_H
