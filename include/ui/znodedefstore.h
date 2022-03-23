//
// Created by lukas on 3/22/22.
//

#ifndef ZPATH_ZNODEDEFSTORE_H
#define ZPATH_ZNODEDEFSTORE_H

#include <map>
#include "ui/nodetype.h"
#include <vector>

class ZNodeDefStore {
public:
    static ZNodeDefStore& get(){
        static ZNodeDefStore instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    vector<NodeType*> getNodeTypes() {
        return mNodeTypes;
    }

    NodeType* getNodeType(const string& name) {
        return mNameMap.at(name);
    }

private:
    vector<NodeType*> mNodeTypes;
    map<string, NodeType*> mNameMap;

    ZNodeDefStore() {

        /// Sin node
        mNodeTypes.push_back(NodeType::fromFile("math/sin.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float chartStart,
                float chartWidth,
                ZNodeView* nodeView) {

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
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {

            complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
            complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
            complex<float> in2 = {x.at(REAL).at(2), x.at(IMAG).at(2)};
            complex<float> out0 = cos(in0 * in1) * in2;

            vector<vector<float>> returnValue =
                    {{out0.real(), start, width},
                     {out0.imag(), start, width}};
            return returnValue;
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/tan.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {

            complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};

            complex<float> out0 = tan(in0);
            vector<vector<float>> returnValue = {
                {out0.real(), start, width},
                {out0.imag(), start, width}
            };
            return returnValue;
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/x.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {

            x.at(REAL).at(0) = x.at(REAL).at(0);
            x.at(REAL).at(1) = start;
            x.at(REAL).at(2) = width;

            x.at(IMAG).at(0) = 0;
            x.at(IMAG).at(1) = start;
            x.at(IMAG).at(2) = width;
            return x;
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/y.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {
            vector<vector<float>> returnValue = {
                    {x.at(IMAG).at(0), start, width},
                        {0, start, width}};
            return returnValue;

        }));

        mNodeTypes.push_back(NodeType::fromFile("math/z.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {
            vector<vector<float>> returnValue =
                    {{x.at(REAL).at(0), start, width},
                        {x.at(IMAG).at(0), start, width}};
            return returnValue;
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/fft.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {

            vector<float> in = x.at(0);

            auto fft = nodeView->computeFft(in.at(1), in.at(2), in.at(3));
            vector<vector<float>> returnValue =  {{fft.first,  width, in.at(3)},
                        {fft.second, width, in.at(3)}};

            return returnValue;

        }));

        mNodeTypes.push_back(NodeType::fromFile("math/hartley.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {

            vector<float> in = x.at(0);

            auto fft = nodeView->computeFft(in.at(1), in.at(2), in.at(3));
            vector<vector<float>> returnValue =
                    {{sqrt(pow(fft.first, 2.0f) + pow(fft.second, 2.0f)),
                         width, in.at(3)},
                        {0.0, width, in.at(3)}};

            return returnValue;
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/ifft.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {

            vector<float> in = x.at(0);
            auto fft = nodeView->computeInverseFft(
                    in.at(1), in.at(2), in.at(3));
                vector<vector<float>> returnValue = {{fft.first,  start, width},
                        {fft.second, start, width}};

            return returnValue;

        }));


        // Index nodes for name lookup
        indexNodes();
    }


    void indexNodes() {
        for (NodeType* nodeType : mNodeTypes) {
            mNameMap.insert({nodeType->mName, nodeType});
        }
    }

};


#endif //ZPATH_ZNODEDEFSTORE_H
