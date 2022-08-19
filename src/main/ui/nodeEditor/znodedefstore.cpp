//
// Created by lukas on 3/26/22.
//

#include "ui/znodedefstore.h"
#include <random>

ZNodeDefStore::ZNodeDefStore() {

    /// Sin node
    mMathNodeTypes.push_back(NodeType::fromFile("math/sin.json", [](FuncIn fn) {
        complex<float> in0 = {fn.x.at(REAL).at(0), fn.x.at(IMAG).at(0)};
        complex<float> in1 = {fn.x.at(REAL).at(1), 0.0};
        complex<float> in2 = {fn.x.at(REAL).at(2), 0.0};
        complex<float> out0 = sin(in0 * in1) * in2;
        fn.x.at(REAL) = {out0.real(), fn.start, fn.width};
        fn.x.at(IMAG) = {out0.imag(), fn.start, fn.width};
        return fn.x;
    }));

    /// Cos node
    mMathNodeTypes.push_back(NodeType::fromFile("math/cos.json", [](FuncIn fn) {
        complex<float> in0 = {fn.x.at(REAL).at(0), fn.x.at(IMAG).at(0)};
        complex<float> in1 = {fn.x.at(REAL).at(1), 0.0};
        complex<float> in2 = {fn.x.at(REAL).at(2), 0.0};
        complex<float> out0 = cos(in0 * in1) * in2;
        fn.x.at(REAL) = {out0.real(), fn.start, fn.width};
        fn.x.at(IMAG) = {out0.imag(), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/tan.json", [](FuncIn fn) {
        complex<float> in0 = {fn.x.at(REAL).at(0), fn.x.at(IMAG).at(0)};

        complex<float> out0 = tan(in0);
        fn.x.at(REAL) = {out0.real(), fn.start, fn.width};
        fn.x.at(IMAG) = {out0.imag(), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/add.json", [](FuncIn fn) {
        complex<float> in0 = {fn.x.at(REAL).at(0), fn.x.at(IMAG).at(0)};
        complex<float> in1 = {fn.x.at(REAL).at(1), fn.x.at(IMAG).at(1)};
        complex<float> out0 = in0 + in1;

        fn.x.at(REAL) = {out0.real(), fn.start, fn.width};
        fn.x.at(IMAG) = {out0.imag(), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/subtract.json", [](FuncIn fn) {
        complex<float> in0 = {fn.x.at(REAL).at(0), fn.x.at(IMAG).at(0)};
        complex<float> in1 = {fn.x.at(REAL).at(1), fn.x.at(IMAG).at(1)};
        complex<float> out0 = in0 - in1;

        fn.x.at(REAL) = {out0.real(), fn.start, fn.width};
        fn.x.at(IMAG) = {out0.imag(), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/multiply.json", [](FuncIn fn) {
        complex<float> in0 = {fn.x.at(REAL).at(0), fn.x.at(IMAG).at(0)};
        complex<float> in1 = {fn.x.at(REAL).at(1), fn.x.at(IMAG).at(1)};
        complex<float> out0 = in0 * in1;

        fn.x.at(REAL) = {out0.real(), fn.start, fn.width};
        fn.x.at(IMAG) = {out0.imag(), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/divide.json", [](FuncIn fn) {
        complex<float> in0 = {fn.x.at(REAL).at(0), fn.x.at(IMAG).at(0)};
        complex<float> in1 = {fn.x.at(REAL).at(1), fn.x.at(IMAG).at(1)};
        complex<float> out0 = in0 / in1;

        fn.x.at(REAL) = {out0.real(), fn.start, fn.width};
        fn.x.at(IMAG) = {out0.imag(), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/pow.json", [](FuncIn fn) {
        complex<float> in0 = {fn.x.at(REAL).at(0), fn.x.at(IMAG).at(0)};
        complex<float> in1 = {fn.x.at(REAL).at(1), fn.x.at(IMAG).at(1)};
        complex<float> out0 = pow(in0, in1);
        fn.x.at(REAL) = {out0.real(), fn.start, fn.width};
        fn.x.at(IMAG) = {out0.imag(), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/sqrt.json", [](FuncIn fn) {
        complex<float> in0 = {fn.x.at(REAL).at(0), fn.x.at(IMAG).at(0)};
        complex<float> out0 = sqrt(in0);
        fn.x.at(REAL) = {out0.real(), fn.start, fn.width};
        fn.x.at(IMAG) = {out0.imag(), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/abs.json", [](FuncIn fn) {
        complex<float> in0 = {fn.x.at(REAL).at(0), fn.x.at(IMAG).at(0)};
        complex<float> out0 = abs(in0);
        fn.x.at(REAL) = {out0.real(), fn.start, fn.width};
        fn.x.at(IMAG) = {out0.imag(), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/exp.json", [](FuncIn fn) {
        complex<float> in0 = {fn.x.at(REAL).at(0), fn.x.at(IMAG).at(0)};
        complex<float> out0 = exp(in0);
        fn.x.at(REAL) = {out0.real(), fn.start, fn.width};
        fn.x.at(IMAG) = {out0.imag(), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/min.json", [](FuncIn fn) {
        fn.x.at(REAL) = {std::min(fn.x.at(REAL).at(0), fn.x.at(REAL).at(1)), fn.start, fn.width};
        fn.x.at(IMAG) = {std::min(fn.x.at(IMAG).at(0), fn.x.at(IMAG).at(1)), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/max.json", [](FuncIn fn) {
        fn.x.at(REAL) = {std::max(fn.x.at(REAL).at(0), fn.x.at(REAL).at(1)), fn.start, fn.width};
        fn.x.at(IMAG) = {std::max(fn.x.at(IMAG).at(0), fn.x.at(IMAG).at(1)), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/random.json", [](FuncIn fn) {
        mt19937_64 rng((int) fn.x.at(REAL).at(0));
        uniform_real_distribution<double> unif;

        float frand = ((float) unif(rng));
        fn.x.at(REAL) = {(float) frand, fn.start, fn.width};
        fn.x.at(IMAG) = {0, fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/gaussian.json", [](FuncIn fn) {
        complex<float> in0 = {fn.x.at(REAL).at(0), 0};
        complex<float> in1 = {fn.x.at(REAL).at(1), 0};
        complex<float> in2 = {fn.x.at(REAL).at(2), 0};
        complex<float> two = {2.0, 0};
        complex<float> out0 = (in2 * exp(-pow(in0, two) / pow(two * in1, two)));
        fn.x.at(REAL) = {out0.real(), fn.start, fn.width};
        fn.x.at(IMAG) = {out0.imag(), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/morlet.json", [](FuncIn fn) {
        vector<float> in = fn.x.at(0);
        auto real = (float) (
                cos(in.at(0) * in.at(4)) * // sinusoid
                (in.at(2) * exp(-pow(in.at(0) - in.at(3), 2) /
                                pow(2 * in.at(1), 2))));

        auto imaginary = (float) (
                sin(in.at(0) * in.at(4)) * // sinusoid
                (in.at(2) * exp(-pow(in.at(0) - in.at(3), 2) /
                                pow(2 * in.at(1), 2))));
        fn.x.at(REAL) = {real, fn.start, fn.width};
        fn.x.at(IMAG) = {imaginary, fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/sigmoid.json", [](FuncIn fn) {
        float in = fn.x.at(REAL).at(0);
        float out = 1.0f / (1.0 + exp(-in));
        fn.x.at(REAL) = {out, fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/tanh.json", [](FuncIn fn) {
        float in = fn.x.at(REAL).at(0);
        float out = tanh(in);
        fn.x.at(REAL) = {out, fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/relu.json", [](FuncIn fn) {
        float in = fn.x.at(REAL).at(0);
        float val = in * 0.3;
        if (in > 0.0) {
            val = in;
        }

        fn.x.at(REAL) = {val, fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/firstdiff.json", [](FuncIn fn) {
        vector<float> in = fn.x.at(0);
        float diff = fn.nodeView->computeFirstDifference(in.at(0), in.at(1));
        fn.x.at(REAL) = {diff, fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/dot.json", [](FuncIn fn) {
        fn.x.at(REAL) = {dot(vec2(fn.x.at(0).at(0), fn.x.at(1).at(0)),
                             vec2(fn.x.at(0).at(1), fn.x.at(1).at(1))), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/cross.json", [](FuncIn fn) {
        fn.x.at(REAL) = {dot(vec2(fn.x.at(0).at(0), fn.x.at(1).at(0)),
                             vec2(fn.x.at(0).at(1), fn.x.at(1).at(1))), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/poly.json", [](FuncIn fn) {
        float in0 = fn.x.at(REAL).at(0);
        float term0 = fn.x.at(REAL).at(1);
        float term1 = fn.x.at(REAL).at(2);
        float term2 = fn.x.at(REAL).at(3);
        float term3 = fn.x.at(REAL).at(4);

        float out0 = (term3 * pow(in0, 3)) + (term2 * pow(in0, 2)) + (term1 * in0) + term0;

        fn.x.at(REAL).at(0) = out0;
        fn.x.at(REAL).at(1) = fn.start;
        fn.x.at(REAL).at(2) = fn.width;
        fn.x.at(IMAG).at(0) = 0.0;
        fn.x.at(IMAG).at(1) = fn.start;
        fn.x.at(IMAG).at(2) = fn.width;
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/chart2d.json", [](FuncIn fn) {
        fn.nodeView->mChart->setResolution(100);
        vector<float> in = fn.x.at(0);

        fn.x.at(REAL).at(0) = in.at(0);
        fn.x.at(REAL).at(1) = in.at(1);
        fn.x.at(REAL).at(2) = fn.start;
        fn.x.at(REAL).at(3) = fn.width;

        fn.x.at(IMAG).at(0) = 0.0;
        fn.x.at(IMAG).at(1) = 0.0;
        fn.x.at(IMAG).at(2) = 0.0;
        fn.x.at(IMAG).at(3) = 0.0;
        return fn.x;
    }));



    mMathNodeTypes.push_back(NodeType::fromFile("math/combine.json", [](FuncIn fn) {
        vector<vector<float>> returnValue;
        for (int i = 0; i < MAX_OUTPUT_COUNT; i++) {
            returnValue.push_back({fn.x.at(REAL).at(i)});
        }
        return returnValue;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/split.json", [](FuncIn fn) {
        vector<vector<float>> returnValue;
        returnValue.push_back(vector<float>( MAX_OUTPUT_COUNT, 0));
        returnValue.push_back(vector<float>( MAX_OUTPUT_COUNT, 0));
        for (int i = 0; i < fn.x.size(); i++) {
            returnValue.at(REAL).at(i) = (fn.x.at(i).at(0));
        }
        return returnValue;
    }));


    mMathNodeTypes.push_back(NodeType::fromFile("math/file.json", [](FuncIn fn) {

        int fileIndex = fn.nodeView->mDropDown->getSelectedItem();
        float point = DataStore::get().getDataAtIndex(fileIndex, fn.x.at(REAL).at(0));
        fn.x.at(REAL) = {point, fn.start, fn.width};
        fn.x.at(IMAG) = {0, fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/heatmap.json", [](FuncIn fn) {
        fn.nodeView->mChart->setResolution(100);
        vector<float> in = fn.x.at(0);
        fn.nodeView->mChart->setZBound(vec2(fn.x.at(0).at(1), fn.x.at(0).at(2)));
        vector<vector<float>> returnValue = {{fn.x.at(0).at(0), fn.start, fn.width}};
        fn.x.at(REAL) = {fn.x.at(0).at(0), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/image.json", [](FuncIn fn) {
        fn.nodeView->mChart->setResolution(200);
        int fileIndex = fn.nodeView->mDropDown->getSelectedItem();
        vec4 point = DataStore::get().getPixelAtIndex(fileIndex, fn.rootInput.at(REAL).at(0), fn.rootInput.at(IMAG).at(0));
        fn.nodeView->mChart->setZBound(vec2(fn.x.at(0).at(1), fn.x.at(0).at(2)));

        fn.x.at(REAL) = {point.r, fn.start, fn.width};
        fn.x.at(IMAG) = {point.g, fn.start, fn.width};
        fn.x.at(2) = {point.b, fn.start, fn.width};
        fn.x.at(3) = {point.a, fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/rgb-out.json", [](FuncIn fn) {

        fn.nodeView->mChart->setResolution(200);
        vec4 rgb = vec4(0);
        rgb.r =  fn.x.at(0).at(0);
        rgb.g =  fn.x.at(1).at(0);
        rgb.b =  fn.x.at(2).at(0);
        rgb.a =  fn.x.at(3).at(0);

        fn.x.at(REAL) = {rgb.r, fn.start, fn.width};
        fn.x.at(IMAG) = {rgb.g, fn.start, fn.width};
        fn.x.at(2) = {rgb.b, fn.start, fn.width};
        fn.x.at(3) = {rgb.a, fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/rgb-convolve.json", [](FuncIn fn) {
        fn.nodeView->mChart->setResolution(200);
        vec4 rgb = vec4(0);
        rgb.r =  fn.x.at(0).at(0);
        rgb.g =  fn.x.at(1).at(0);
        rgb.b =  fn.x.at(2).at(0);
        rgb.a =  fn.x.at(3).at(0);

        fn.x.at(REAL) = {rgb.r, fn.start, fn.width};
        fn.x.at(IMAG) = {rgb.g, fn.start, fn.width};
        fn.x.at(2) = {rgb.b, fn.start, fn.width};
        fn.x.at(3) = {rgb.a, fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/rgbmul.json", [](FuncIn fn) {
        fn.nodeView->mChart->setResolution(100);
        vec4 rgb = vec4(0);
        rgb.r =  fn.x.at(0).at(0);
        rgb.g =  fn.x.at(1).at(0);
        rgb.b =  fn.x.at(2).at(0);
        rgb.a =  fn.x.at(3).at(0);

        vec4 rgb2 = vec4(0);
        rgb2.r =  fn.x.at(0).at(1);
        rgb2.g =  fn.x.at(1).at(1);
        rgb2.b =  fn.x.at(2).at(1);
        rgb2.a =  fn.x.at(3).at(1);

        fn.x.at(REAL) = {rgb.r * rgb2.r, fn.start, fn.width};
        fn.x.at(IMAG) = {rgb.g * rgb2.g, fn.start, fn.width};
        fn.x.at(2) = {rgb.b * rgb2.b, fn.start, fn.width};
        fn.x.at(3) = {rgb.a * rgb2.a, fn.start, fn.width};
        return fn.x;
    }));


    mMathNodeTypes.push_back(NodeType::fromFile("math/rgbadd.json", [](FuncIn fn) {
        fn.nodeView->mChart->setResolution(100);
        vec4 rgb = vec4(0);
        rgb.r =  fn.x.at(0).at(0);
        rgb.g =  fn.x.at(1).at(0);
        rgb.b =  fn.x.at(2).at(0);
        rgb.a =  fn.x.at(3).at(0);

        vec4 rgb2 = vec4(0);
        rgb2.r =  fn.x.at(0).at(1);
        rgb2.g =  fn.x.at(1).at(1);
        rgb2.b =  fn.x.at(2).at(1);
        rgb2.a =  fn.x.at(3).at(1);

        rgb2.a =  fn.x.at(3).at(0);
        fn.x.at(REAL) = {rgb.r + rgb2.r, fn.start, fn.width};
        fn.x.at(IMAG) = {rgb.g + rgb2.g, fn.start, fn.width};
        fn.x.at(2) = {rgb.b + rgb2.b, fn.start, fn.width};
        fn.x.at(3) = {rgb.a + rgb2.a, fn.start, fn.width};
        return fn.x;
    }));


    mMathNodeTypes.push_back(NodeType::fromFile("math/c.json", [](FuncIn fn) {
        fn.x.at(REAL) = fn.nodeView->mConstantValueOutput;
        fn.x.at(IMAG).at(0) = 0;
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/ci.json", [](FuncIn fn) {
        fn.x.at(IMAG) = fn.nodeView->mConstantValueOutput;
        fn.x.at(REAL).at(0) = 0;
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/t.json", [](FuncIn fn) {
        fn.x.at(REAL).at(0) = glfwGetTime() * fn.x.at(REAL).at(0);
        fn.nodeView->invalidateNodeRecursive();
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/x.json", [](FuncIn fn) {

        fn.x.at(REAL).at(0) = fn.x.at(REAL).at(0);
        fn.x.at(REAL).at(1) = fn.start;
        fn.x.at(REAL).at(2) = fn.width;

        fn.x.at(IMAG).at(0) = 0;
        fn.x.at(IMAG).at(1) = fn.start;
        fn.x.at(IMAG).at(2) = fn.width;
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/y.json", [](FuncIn fn) {
        vector<vector<float>> returnValue = {
                {fn.x.at(IMAG).at(0), fn.start, fn.width},
                {0, fn.start, fn.width}};
        return returnValue;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/z.json", [](FuncIn fn) {
        vector<vector<float>> returnValue =
                {{fn.x.at(REAL).at(0), fn.start, fn.width},
                 {fn.x.at(IMAG).at(0), fn.start, fn.width}};
        return returnValue;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/fft.json", [](FuncIn fn) {
        vector<float> in = fn.x.at(0);
        auto fft = fn.nodeView->computeFft(in.at(1), in.at(2), in.at(3));
        fn.x.at(REAL) = {fft.first,  fn.width, in.at(3)};
        fn.x.at(IMAG) = {fft.second, fn.width, in.at(3)};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/hartley.json", [](FuncIn fn) {
        vector<float> in = fn.x.at(0);
        auto fft = fn.nodeView->computeFft(in.at(1), in.at(2), in.at(3));
        fn.x.at(REAL) = {sqrt(pow(fft.first, 2.0f) + pow(fft.second, 2.0f)),
                         fn.width, in.at(3)};
        fn.x.at(IMAG) = {0.0, fn.width, in.at(3)};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/ifft.json", [](FuncIn fn) {

        vector<float> in = fn.x.at(0);
        auto fft = fn.nodeView->computeInverseFft(
                in.at(1), in.at(2), in.at(3));
        fn.x.at(REAL) = {fft.first,  fn.width, in.at(3)};
        fn.x.at(IMAG) = {fft.second, fn.width, in.at(3)};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/group.json", [](FuncIn fn) {
        fn.nodeView->initializeGroup();
        return fn.nodeView->mGroupOutput->evaluate(fn.rootInput, nullptr, fn.rootInput);
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/neural-group.json", [](FuncIn fn) {
        fn.nodeView->initializeGroup();
        return fn.nodeView->mGroupOutput->evaluate(fn.rootInput, nullptr, fn.rootInput);
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/in.json", [](FuncIn fn) {
            if (fn.nodeView->mInputProxy != nullptr) {
                return fn.nodeView->mInputProxy->sumAllInputs(
                        fn.x, nullptr, vector<vector<float>>());
            } else {
                return fn.x;
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

    mMathNodeTypes.push_back(NodeType::fromFile("math/out.json", [](FuncIn fn) {
        return fn.x;
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

    mMathNodeTypes.push_back(NodeType::fromFile("math/neuralcore.json", [](FuncIn fn) {
        fn.nodeView->initializeGroup();
        if (fn.nodeView->mMlModel == nullptr) {
            fn.nodeView->initializeNNModel();
        }

        float returnValue;
        if (fn.nodeView->mMlModel->getTrainingInProgress()) {
            vec2 thisChartBounds = fn.nodeView->mChart->getXBounds();
            float span = thisChartBounds.y - thisChartBounds.x;
            float inX = ((fn.x.at(REAL).at(1) - thisChartBounds.x) / span) * fn.cache.size();
            if (span > 0) {
                int xIndex = 0;
                if (inX >= 0 && (inX) < fn.cache.size() && !fn.cache.empty()) {
                    xIndex = (int) inX;
                    complex<float> y = fn.cache.at(xIndex);
                    returnValue = y.real();
                }
            }

        } else {
            fn.nodeView->mMlModel->setInput(fn.x.at(REAL).at(1), 0);
            fn.nodeView->mMlModel->compute();
            returnValue = fn.nodeView->mMlModel->getOutputAt(0);
        }

        fn.x.at(IMAG) = {fn.x.at(REAL).at(0),fn.start, fn.width};
        fn.x.at(REAL) = {returnValue, fn.start, fn.width};
        return fn.x;

    }, {
        [](ZNodeView* sender) -> void {
            // Train the network
            sender->trainNN(sender->mButtons.at(0));
        },
        [](ZNodeView* sender) -> void {
            sender->mMlModel->resetNetwork();
        },
        [](ZNodeView* sender) -> void {
            cout<< sender->mMlModel->toFunctionString() << endl;
        },[](ZNodeView* sender) -> void {
            cout<< sender->mMlModel->toFunctionString() << endl;
        }
    }, [](ZNodeView* nodeView){

        for (auto groupNode : nodeView->getGroupNodes()) {
            if (groupNode->getType()->mName != "in" && groupNode->getType()->mName != "out") {
                groupNode->setVisibility(false);
                ZNodeUtil::get().deleteNode(groupNode);
            }
        }
        
        // On Enter group
        auto nnNodes = ZNodeUtil::get().nodesFromMlModel(nodeView->mMlModel,
                                                         nodeView->mGroupInput, nodeView->mGroupOutput);


        for (auto node : nnNodes) {
            nodeView->getAddNodeInterface()(node, true);
            nodeView->addGroupNode(node);
            nodeView->setVisibility(true);
        }

    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/laplace.json", [](FuncIn fn) {
        vector<float> in = fn.x.at(0);
        fn.nodeView->mChart->setZBound(vec2(fn.x.at(REAL).at(4), fn.x.at(REAL).at(5)));
        // Static resolution for now
        auto laplace = fn.nodeView->computeLaplace(fn.x.at(REAL).at(1),
                                                   fn.x.at(IMAG).at(1),
                                                   in.at(2), in.at(3),
                                                   fn.nodeView->mChart->getMaxResolution());
        fn.x.at(REAL).at(0) = laplace.first;
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/laplace-s.json", [](FuncIn fn) {
        vector<vector<float>> sx = fn.nodeView->computeLaplaceHeadless(
                {vector<float>(MAX_INPUT_COUNT, fn.x.at(REAL).at(1)),
                 vector<float>(MAX_INPUT_COUNT, fn.x.at(IMAG).at(1))}, fn.rootInput);

        fn.nodeView->mChart->setZBound(vec2(fn.x.at(0).at(2), fn.x.at(0).at(3)));
        fn.x.at(REAL) = {sx.at(0).at(0), fn.start, fn.width};
        return fn.x;
    }));

    mMathNodeTypes.push_back(NodeType::fromFile("math/comment.json"));

    string editorFolder = "json/";
    string projectFolder = "resources/node-def/" + editorFolder;
    string path = ZSettings::get().getResourcePath() + projectFolder;
    vector<string> names;
    for (const auto &file : directory_iterator(path)) {
        names.push_back(editorFolder + getFileName(file.path()));
    }

    for (string name : names) {
        if (name == "json/output.json") {
            mJsonNodeTypes.push_back(NodeType::fromFile(name, [](FuncIn fn) {
                string path = ZNodeUtil::get().nodeToJsonPath(fn.nodeView);
                if (path != fn.nodeView->getText()) {
                    fn.nodeView->setText(path);
                }
                return fn.x;
            }));
        } else {
            mJsonNodeTypes.push_back(NodeType::fromFile(name));
        }

    }


    // Index nodes for name lookup
    indexNodes();
}
