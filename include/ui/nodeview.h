//
// Created by lukas on 10/4/20.
//

#ifndef ZPATH_NODEVIEW_H
#define ZPATH_NODEVIEW_H

static const int SOCKET_SIZE = 15;
static const int MAX_INPUT_COUNT = 4;
static const int MAX_OUTPUT_COUNT = 4;
static const int CHART_RES_THRESHOLD = 4;
static const int CHART_SIDE_MARGIN_WIDE = 20;
static const int MIN_MARGIN = 3;
static const int CHART_TOP_MARGIN = 15;
using namespace std;
#include <vector>
#include "zview.h"
#include "zchart.h"
#include "zlinechart.h"

class ZNodeView : public ZView {
public:

    ZNodeView(float maxWidth, float maxHeight, ZView *parent);
    void setOnValueSelect(function<void(ZLabel* sender, ZNodeView* node)> onValueSelect);
    void setInvalidateListener(function<void(ZNodeView* node)> listener);

    enum Type {
        SIN,
        COS,
        TAN,
        EXP,
        SQRT,
        POW,
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        C, // Constant value
        X,
        Y,
        FILE,
        FFT,
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

    vector<float> evaluate(vector<float> x);
    vector<float> evaluate(vector<float> x, ZNodeView* root);

    vector<float> compute(const vector<float>& in, Type type) {
        vec2 chartBound = mChart->getXBounds();
        switch (type) {
            case SIN:
                return {sin(in.at(0)), chartBound.x, chartBound.y};
            case COS:return {cos(in.at(0)), chartBound.x, chartBound.y};
            case TAN:return {tan(in.at(0)), chartBound.x, chartBound.y};
            case EXP:return {exp(in.at(0)), chartBound.x, chartBound.y};
            case SQRT:return {sqrt(in.at(0)), chartBound.x, chartBound.y};
            case POW:return {pow(in.at(0), in.at(1)), chartBound.x, chartBound.y};
            case ADD:return {in.at(0) + in.at(1), chartBound.x, chartBound.y};
            case SUBTRACT:return {in.at(0) - in.at(1), chartBound.x, chartBound.y};
            case MULTIPLY:return {in.at(0) * in.at(1), chartBound.x, chartBound.y};
            case DIVIDE:return {in.at(0) / in.at(1), chartBound.x, chartBound.y};
            case C:return mConstantValue;
            case X:return {in.at(0), chartBound.x, chartBound.y};
            case Y:return {in.at(1), chartBound.x, chartBound.y};
            case FILE:break;
            case FFT:break;
            case LAPLACE:break;
            case FIRST_DIFF:break;
            case SECOND_DIFF:break;
            case DOT:break;
            case CROSS:break;
            case CHART_2D: return {in.at(0), in.at(1), chartBound.x, chartBound.y};
            case LAST:break;
        }

        return vector<float>(3);
    }

    /**
     * First value is input count, second value is output count. If input count is zero node
     * is an input node like a constant or number range. If output count is zero the node is
     * an output node like a plot or label.
     * @return Vector with node socket input and output count
     */
    ivec2 getSocketCount() {
        switch (mType) {

            case SIN:return ivec2(1,3);
            case COS:return ivec2(1,3);
            case TAN:return ivec2(1,3);
            case EXP:return ivec2(1,3);
            case SQRT:return ivec2(1,3);
            case POW:return ivec2(2,3);
            case ADD:return ivec2(2,3);
            case SUBTRACT:return ivec2(2,3);
            case MULTIPLY:return ivec2(2,3);
            case DIVIDE:return ivec2(2,3);
            case C:return ivec2(0,1);
            case X:return ivec2(0,3);
            case Y:return ivec2(0,3);
            case FILE:return ivec2(0,1);
            case FFT:return ivec2(3,3);
            case LAPLACE:return ivec2(3,3);
            case FIRST_DIFF:return ivec2(3,3);
            case SECOND_DIFF:return ivec2(3,3);
            case DOT:return ivec2(4,3);
            case CROSS:return ivec2(4,4);
            case CHART_2D: return ivec2(2,4);
            case LAST:return ivec2(0,0);
        }
    }

    pair<vector<SocketType>,vector<SocketType>>  getSocketType() {
        switch (mType) {
            case SIN:
            case COS:
            case TAN:
            case EXP:
            case SQRT:
            case POW:return {{VAR}, {VAR, CON, CON}};
            case ADD:
            case SUBTRACT:
            case MULTIPLY:
            case DIVIDE:return {{VAR, VAR}, {VAR, CON, CON}};
            case C:return {{}, {CON}};
            case X:
            case Y:return {{}, {VAR, CON, CON}};
            case FILE:return {{}, {VAR, CON, CON}};
            case FFT:return {{VAR, CON, CON}, {VAR, CON, CON}};
            case LAPLACE:return {{VAR, CON, CON}, {VAR, CON, CON}};
            case FIRST_DIFF:return {{VAR, CON, CON}, {VAR, CON, CON}};
            case SECOND_DIFF:return {{VAR, CON, CON}, {VAR, CON, CON}};
            case DOT:return {{VAR, VAR, VAR, VAR}, {VAR, CON, CON}};
            case CROSS:return  {{VAR, VAR, VAR, VAR}, {VAR, VAR, CON, CON}};
            case CHART_2D: return {{VAR, VAR}, {VAR, VAR, CON, CON}};
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
            case ADD:return "+";
            case SUBTRACT:return "-";
            case MULTIPLY:return "*";
            case DIVIDE:return "/";
            case C:return "C";
            case X:return "x";
            case Y:return "y";
            case FILE:return "file";
            case FFT:return "FFT";
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
                return vec2(200, 200);
            default:
                return vec2(80, 100);
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
    void setConstantValue(vector<float> value);

    void invalidateSingleNode();
    void invalidateNodeRecursive();

    void initializeEdges();
private:
    bool mInvalid = true;

    vector<ZView*> mSocketsIn;
    vector<ZView*> mSocketsOut;

    Type mType = ADD;

    ZLabel* mOutputLabel;
    ZLabel* mNameLabel;

    /**
     * Number of line segments on the chart
     */

    // Todo: remove  these
    ZLineChart* mChart;
    vector<vector<float>> mPointCache;

    vector<float> mConstantValue = {0.0};
    vec4 mVariableColor = vec4(1, 0.611956, 0.052950, 1);
    vec4 mConstantColor = vec4(1, 0.437324, 0.419652, 1);

    function<void(ZLabel* sender, ZNodeView* node)> mListener;
    function<void(ZNodeView* node)> mInvalidateListener;

    bool onMouseEvent(int button, int action, int mods, int sx, int sy) override;

    void clearInvalidateNode();
    bool isInvalid();

    void updateChart1D();
    void updateChart2D();
};






#endif //ZPATH_NODEVIEW_H
