//
// Created by lukas on 3/22/22.
//

#ifndef ZPATH_ZMATHNODEDEF_H
#define ZPATH_ZMATHNODEDEF_H

#include <map>
#include "ui/nodetype.h"
#include <vector>

class ZMathNodeDef {
public:
    static ZMathNodeDef& get(){
        static ZMathNodeDef instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    vector<NodeType> getNodeTypes() {
        return mNodeTypes;
    }

private:
    vector<NodeType> mNodeTypes;

    ZMathNodeDef() {

        /// Sin node
        mNodeTypes.push_back(NodeType::fromFile("math/sin.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                vector<float>& cache,
                float chartStart,
                float chartWidth) {

            complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
            complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
            complex<float> in2 = {x.at(REAL).at(2), x.at(IMAG).at(2)};
            complex<float> out0 = sin(in0 * in1) * in2;

            vector<vector<float>> returnValue =
                    {{out0.real(), chartStart, chartWidth},
                     {out0.imag(), chartStart, chartWidth}};
            return returnValue;
        }));

        /// Cos node
        mNodeTypes.push_back(NodeType::fromFile("math/cos.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                vector<float>& cache,
                float chartStart,
                float chartWidth) {

            complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
            complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
            complex<float> in2 = {x.at(REAL).at(2), x.at(IMAG).at(2)};
            complex<float> out0 = cos(in0 * in1) * in2;

            vector<vector<float>> returnValue =
                    {{out0.real(), chartStart, chartWidth},
                     {out0.imag(), chartStart, chartWidth}};
            return returnValue;
        }));


    }

};


#endif //ZPATH_ZMATHNODEDEF_H
