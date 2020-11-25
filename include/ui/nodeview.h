//
// Created by lukas on 10/4/20.
//

#ifndef ZPATH_NODEVIEW_H
#define ZPATH_NODEVIEW_H

static const int SOCKET_SIZE = 12;
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
        CROSS
    };

    vector<ZNodeView*> mInputs;
    Type mType;



    vector<ZView*> getSocketsIn();
    vector<ZView*> getSocketsOut();
private:
    vector<ZView*> mSocketsIn;
    vector<ZView*> mSocketsOut;


    void getClostestSocket();
};


#endif //ZPATH_NODEVIEW_H
