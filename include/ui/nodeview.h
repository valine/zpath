//
// Created by lukas on 10/4/20.
//

#ifndef ZPATH_NODEVIEW_H
#define ZPATH_NODEVIEW_H

static const int SOCKET_SIZE = 12;
static const int MAX_INPUT_COUNT = 5;
static const int MAX_OUTPUT_COUNT = 5;
using namespace std;
#include <vector>
#include "zview.h"

class ZNodeView : public ZView {


public:

    ZNodeView(float maxWidth, float maxHeight, ZView *parent);

    enum Type {
        SIN,
        COS,
        TAN,
        EXP,
        SQRT,
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        VALUE, // Constant value
        RANGE, // Complex number range
        FILE,
        FFT,
        LAPLACE,
        FIRST_DIFF,
        SECOND_DIFF,
        DOT,
        CROSS,
    };

    vector<float> compute(const vector<float>& in) {
        switch (mType) {
            case SIN:return {sin(in.at(0))};
            case COS:return {cos(in.at(0))};
            case TAN:return {tan(in.at(0))};
            case EXP:return {exp(in.at(0))};
            case SQRT:return {sqrt(in.at(0))};
            case ADD:return {in.at(0) + in.at(1)};
            case SUBTRACT:return {in.at(0) - in.at(1)};
            case MULTIPLY:return {in.at(0) * in.at(1)};
            case DIVIDE:return {in.at(0) / in.at(1)};
            case VALUE:return {3}; // TODO: This needs to pull from slider UI
            case RANGE:return {in.at(0)};
            case FILE:break;
            case FFT:break;
            case LAPLACE:break;
            case FIRST_DIFF:break;
            case SECOND_DIFF:break;
            case DOT:break;
            case CROSS:break;
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
            case SIN:return ivec2(1,1);
            case COS:return ivec2(1,1);
            case TAN:return ivec2(1,1);
            case EXP:return ivec2(1,1);
            case SQRT:return ivec2(1,1);
            case ADD:return ivec2(2,1);
            case SUBTRACT:return ivec2(2,1);
            case MULTIPLY:return ivec2(2,1);
            case DIVIDE:return ivec2(2,1);
            case VALUE:return ivec2(0,1);
            case RANGE:return ivec2(0,1);
            case FILE:return ivec2(0,1);
            case FFT:return ivec2(1,0);
            case LAPLACE:return ivec2(1,0);
            case FIRST_DIFF:return ivec2(1,0);
            case SECOND_DIFF:return ivec2(1,0);
            case DOT:return ivec2(4,1);
            case CROSS:return ivec2(4,2);
        }
    }

    void setType(Type type);

    vector<ZNodeView*> mInputs;
    vector<ZNodeView*> mOutputs;

    vector<vector<pair<ZNodeView*, int>>> mInputIndices;
    vector<vector<pair<ZNodeView*, int>>> mOutputIndices;

    vector<ZView*> getSocketsIn();
    vector<ZView*> getSocketsOut();

    virtual void onMouseEvent(int button, int action, int mods, int x, int y) override;
private:
    vector<ZView*> mSocketsIn;
    vector<ZView*> mSocketsOut;

    Type mType = ADD;
};


#endif //ZPATH_NODEVIEW_H
