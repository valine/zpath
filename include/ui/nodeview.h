//
// Created by lukas on 10/4/20.
//

#ifndef ZPATH_NODEVIEW_H
#define ZPATH_NODEVIEW_H

static const int SOCKET_SIZE = 8;
using namespace std;
#include <vector>
#include "zview.h"

class NodeView : public ZView {


public:

    NodeView(float maxWidth, float maxHeight, ZView *parent);


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
        CROSS
    };

    vector<NodeView*> mInputs;
    Type mType;

private:


};


#endif //ZPATH_NODEVIEW_H
