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

        mNodeTypes.push_back(NodeType::fromFile("math/add.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {
                complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
                complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
                complex<float> out0 = in0 + in1;
                vector<vector<float>> returnValue =   {{out0.real(), start, width},
                        {out0.imag(), start, width}};
                return returnValue;
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/subtract.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {
            complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
            complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
            complex<float> out0 = in0 - in1;
            vector<vector<float>> returnValue =   {{out0.real(), start, width},
                                                   {out0.imag(), start, width}};
            return returnValue;
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/multiply.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {
            complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
            complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
            complex<float> out0 = in0 * in1;
            vector<vector<float>> returnValue =   {{out0.real(), start, width},
                                                   {out0.imag(), start, width}};
            return returnValue;
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/divide.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {
            complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
            complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
            complex<float> out0 = in0 / in1;
            vector<vector<float>> returnValue =   {{out0.real(), start, width},
                                                   {out0.imag(), start, width}};
            return returnValue;
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/pow.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {

            complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
                complex<float> in1 = {x.at(REAL).at(1), x.at(IMAG).at(1)};
                complex<float> out0 = pow(in0, in1);
            vector<vector<float>> returnValue =  {{out0.real(), start, width},
                        {out0.imag(), start, width}};
            return returnValue;
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/sqrt.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {
            complex<float> in0 = {x.at(REAL).at(0), x.at(IMAG).at(0)};
                complex<float> out0 = sqrt(in0);
            vector<vector<float>> returnValue = {{out0.real(), start, width},
                        {out0.imag(), start, width}};
            return returnValue;
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/c.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {
            x.at(REAL) = nodeView->mConstantValueOutput;
            x.at(IMAG).at(0) = 0;
            return x;
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/ci.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {
            x.at(IMAG) = nodeView->mConstantValueOutput;
            x.at(REAL).at(0) = 0;
            return x;
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

        mNodeTypes.push_back(NodeType::fromFile("math/group.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {
            nodeView->initializeGroup();
            return nodeView->mGroupOutput->evaluate(rootInput, nullptr, rootInput);
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/in.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {

            if (nodeView->mInputProxy != nullptr) {
                return nodeView->mInputProxy->sumAllInputs(
                        x, nullptr, vector<vector<float>>());
            } else {
                return x;
            }
        }, {
            [](ZNodeView* sender) -> void {
                // Plus button
                ivec2 count = sender->mSocketCount + ivec2(0,1);
                sender->setSocketCount(count);
            },
            [](ZNodeView* sender) -> void {
                // Minus button
                ivec2 count = sender->mSocketCount - ivec2(0,1);
                sender->setSocketCount(count);
            }
        }));

        mNodeTypes.push_back(NodeType::fromFile("math/out.json", [](
                vector<vector<float>> x,
                const vector<vector<float>>& rootInput,
                const vector<complex<float>>& cache,
                float start,
                float width,
                ZNodeView* nodeView) {
            return x;
        }, {
            [](ZNodeView* sender) -> void {
                // Plus button
                ivec2 count = sender->mSocketCount + ivec2(1,0);
                sender->setSocketCount(count);
            },
            [](ZNodeView* sender) -> void {
                // Minus button
                ivec2 count = sender->mSocketCount - ivec2(1,0);
                sender->setSocketCount(count);
            }
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
