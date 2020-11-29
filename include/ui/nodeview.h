//
// Created by lukas on 10/4/20.
//

#ifndef ZPATH_NODEVIEW_H
#define ZPATH_NODEVIEW_H

static const int SOCKET_SIZE = 15;
static const int MAX_INPUT_COUNT = 5;
static const int MAX_OUTPUT_COUNT = 5;
using namespace std;
#include <vector>
#include "zview.h"

class ZNodeView : public ZView {
public:

    ZNodeView(float maxWidth, float maxHeight, ZView *parent);
    void setOnValueSelect(function<void(ZLabel* sender, ZNodeView* node)> onValueSelect);

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
        LAST // Fake enum to allow easy iteration
    };

    vector<float> evaluate(vector<float> x) {
        ivec2 size = getSocketCount();
        vector<float> output;
        if (x.size() < size.x) {
          //  mOutputLabel->setText(to_string(size.x) + " inputs needed, got " + to_string(x.size()));
          //  return vector<float>();
        } else {
            output = compute(x, mType);
        }

        if (size.x > 0) {
            vector<float> summedInputs = vector<float>((int) size.x);

            // Loop over input sockets
            for (int i = 0; i < size.x; i++) {
                const vector<pair<ZNodeView *, int>> &inputs = mInputIndices.at(i);

                // If inputs are connected evaluate recursively, otherwise use the specified input.
                if (!inputs.empty()) {
                    // Summing all inputs is useful for dot products.
                    float sum = 0;

                    // Loop over all inputs on a single socket
                    for (pair<ZNodeView *, int> input : inputs) {

                        // It's possible a previous node on the stack has too few inputs.
                        // When that happens display an error message.
                        vector<float> recurOutput = input.first->evaluate(x);
                        if (recurOutput.empty()) {
                            mOutputLabel->setText("Bad input");
                            setBackgroundColor(red);
                            mOutputLabel->setBackgroundColor(red);
                            mOutputLabel->setTextColor(white);
                            return vector<float>();
                        } else {
                            sum += input.first->evaluate(x).at(input.second);
                        }
                    }
                    summedInputs.at(i) = sum;
                } else {

                    // Check that the passed input vector dimension matches
                    // the number of input sockets on the node. If not
                    // display an error message.
                    if (x.size() <= size.x) {
                        mOutputLabel->setText(to_string(size.x) + " inputs needed, got " + to_string(x.size()));
                        setBackgroundColor(red);
                        mOutputLabel->setBackgroundColor(red);
                        mOutputLabel->setTextColor(white);
                        return vector<float>();
                    } else {
                        summedInputs.at(i) = x.at(i);
                    }
                }
            }
            output = compute(summedInputs, mType);
        }

        mOutputLabel->setText(to_string(output.at(0)));
        setBackgroundColor(white);
        mOutputLabel->setTextColor(black);
        mOutputLabel->setBackgroundColor(white);
        return output;

    }

    static vector<float> compute(const vector<float>& in, Type type) {
        switch (type) {
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
            case LAST:return ivec2(0,0);
        }
    }

    static string getName(Type type) {
        switch (type) {
            case SIN:return "sin";
            case COS:return "cos";
            case TAN:return "tan";
            case EXP:return "exp";
            case SQRT:return "sqrt";
            case ADD:return "+";
            case SUBTRACT:return "-";
            case MULTIPLY:return "*";
            case DIVIDE:return "/";
            case VALUE:return "constant";
            case RANGE:return "input";
            case FILE:return "file";
            case FFT:return "FFT";
            case LAPLACE:return "Laplace";
            case FIRST_DIFF:return "1st diff";
            case SECOND_DIFF:return "2nd diff";
            case DOT:return "dot";
            case CROSS:return "cross";
            case LAST:return "none";
        }
    }

    void setType(Type type);

    vector<ZNodeView*> mInputs;
    vector<ZNodeView*> mOutputs;

    vector<vector<pair<ZNodeView*, int>>> mInputIndices;
    vector<vector<pair<ZNodeView*, int>>> mOutputIndices;

    vector<ZView*> getSocketsIn();
    vector<ZView*> getSocketsOut();
private:
    vector<ZView*> mSocketsIn;
    vector<ZView*> mSocketsOut;

    Type mType = ADD;

    ZLabel* mOutputLabel;
    ZLabel* mNameLabel;


    function<void(ZLabel* sender, ZNodeView* node)> mListener;


    virtual void onMouseEvent(int button, int action, int mods, int sx, int sy);
};






#endif //ZPATH_NODEVIEW_H
