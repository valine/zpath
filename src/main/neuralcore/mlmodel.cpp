//
// Created by lukas on 4/6/20.
//

#include <neuralcore/sigmoidneuron.h>
#include "neuralcore/mlmodel.h"
#include "neuralcore/squareneuron.h"
#include "neuralcore/cubeneuron.h"
#include <iostream>
#include <neuralcore/costneuron.h>
#include <neuralcore/inputnode.h>
#include <neuralcore/reluneuron.h>
#include <random>
#include <algorithm>
#include <neuralcore/csvutils.h>
#include <functional>

MlModel::MlModel(int width, vector<int> height, int inputs, int outputs, int miniBatchSize, double stepSize,
                 Optimizer optimizer, vector<Neuron::Activation> mTypes) {
    mHiddenWidth = width;
    mHiddenHeight =  height;
    mInputs = inputs;
    mOutputs = outputs;
    mMiniBatchSize = miniBatchSize;
    mStep = stepSize;
    mOptimizer = optimizer;

    double var = 0.0;
    mDist = normal_distribution<double>(0,var);

    vector<Neuron*> totalCost;
    mTotalCostNode = new Neuron(1.0);
    mTotalCostNode->setName("Total Cost");
    totalCost.push_back(mTotalCostNode);

    vector<Neuron*> lastNodes;
    for (int i = 0; i < mOutputs; i++) {
        // Add cost node
        auto* costNode = new CostNeuron();
        costNode->setExpectedValue(1.0);
        costNode->setName("Cost");
        mTotalCostNode->addParent(costNode, 1.0);
        mCostNodes.push_back(costNode);

        auto *output = new Neuron();
        output->setName("Output " + to_string(i));
        lastNodes.push_back(output);
        costNode->addParent(output, 1.0);
        mOutputNodes.push_back(output);
    }

    mLayers.push_back(totalCost);
    mLayers.push_back(mCostNodes);
    mLayers.push_back(mOutputNodes);
    mTrainable.push_back(mOutputNodes);
    for (int w = 0; w < mHiddenWidth; w++) {
        vector<Neuron*> hiddenLayer;
        for (int h = 0; h < mHiddenHeight.at(mHiddenWidth - w - 1); h++) {

            Neuron* node = new Neuron();
            node->setType(mTypes.at(mHiddenWidth - w - 1));

            node->setName("Layer " + to_string(mHiddenWidth - w) + " : " + to_string(h));
            node->setBias(0.0);
            // Connect nodes

            for (Neuron* previous : mLayers.at(mLayers.size() - 1)) {
                double randomWeight = getInitialWeight(mHiddenHeight.at(mHiddenWidth - w - 1));
                previous->addParent(node, randomWeight);
            }

            if ((int) lastNodes.size() - 1 < h) {
                lastNodes.push_back(node);
            } else {
                lastNodes.at(h) = node;
            }

            hiddenLayer.push_back(node);
        }
        mLayers.push_back(hiddenLayer);
        mTrainable.push_back(hiddenLayer);
    }

    for (int i = 0; i < mInputs; i++) {
        auto *input = new InputNode();
        input->setName("Input " + to_string(i));
        input->setValue(1.0);
        mInputNodes.push_back(input);

        // Connect nodes
        for (Neuron* previous : mLayers.at(mLayers.size() - 1)) {
            double randomWeight = getInitialWeight(mInputs);
            previous->addParent(input, randomWeight);
        }
    }

    mLayers.push_back(mInputNodes);
}

void MlModel::addNodeToLayer(int layerIndex) {
    vector<Neuron*> layer = mLayers.at(layerIndex);
    Neuron* node = new Neuron();

    int index = getWidth() - layerIndex - 1;
    node->setType(layer.at(0)->getType());


    node->setName("Layer " + to_string(index - 1) + " : " + to_string(layer.size()));
    node->setBias(0.0);
    // Connect nodes

    for (Neuron* parent : mLayers.at(index + 1)) {
        node->addParent(parent, 0);
    }

    for (Neuron* child : mLayers.at(index - 1)) {
        child->addParent(node, 0);
    }

    mLayers.at(index).push_back(node);
    mTrainable.at(mTrainable.size() - layerIndex - 1).push_back(node);
    mHiddenHeight.at(mHiddenHeight.size() - layerIndex - 1)++;

}

//double MlViewController::pulseWidthComp(double pw) {
//    double input = (-0.743756 * ((pw - (235.0)) / 126.984251));
//    double layer1 = 9.366929 * (((2.0 / (1.0 + exp(-2 * (input + -1.961630)))) - 1.0));
//    double output = ((layer1 + 7.455819) * 1.342497) + (3.700000);
//    return output;
//}

string MlModel::toFunctionString() {
    string data;
    bool singleOutput = mOutputNodes.size() == 1;

    // Generate input parameters
    string inputParams;

    for (unsigned int i = 0; i < mInputNodes.size(); i++) {
        inputParams+="float in" + to_string(i);
        if (i < mInputNodes.size() - 1) {
            inputParams += ", ";
        }
    }

    // Set return type based on number of outputs
    string functionReturnType;
    if (singleOutput) {
        functionReturnType = "float";
    } else {
        functionReturnType = "vector<float>";
    }

    data+= functionReturnType + " run(" + inputParams + ") {\n";


    // Normalize inputs base on median and variance
    for (unsigned int i = 0; i < mInputNodes.size(); i++) {
        string normalizedInput;
        string withMed = "in" + to_string(i) + " - " + to_string(mInputNodes.at(i)->getMedian());
        string withVar =  par(par(withMed) + " / " + to_string(mInputNodes.at(i)->getVariance()));
        normalizedInput+="  float input" + to_string(i) + " = " + withVar + ";\n";
        data+=normalizedInput;
    }


    int layer = 1;
    for (int layerHeight : mHiddenHeight) {
        for (int i = 0; i < layerHeight; i++) {
            Neuron* node = getNodeAt(layer, i);
            string normalizedInput;

            string layerInput;
            for(unsigned int wi = 0; wi < node->getWeights().size(); wi++) {
                // If first hidden layer
                if (layer == 1) {
                    string withWeight = "input" + to_string(wi)  + " * " + to_string(node->getWeights().at(wi));
                    layerInput += par(withWeight);
                } else {
                    string withWeight = "layer" + to_string(layer - 1) + "x" + to_string(wi)  + " * " + to_string(node->getWeights().at(wi));
                    layerInput += par(withWeight);

                }

                if (wi < node->getWeights().size() - 1) {
                    layerInput += " + ";
                }
            }

            string layerValue = par(layerInput) + " + " + to_string(node->getBias());
            string activation = getActivationName(node->getType()) + par(layerValue);

            normalizedInput += "  float layer" + to_string(layer) + "x" + to_string(i) + " = " + activation + ";\n";
            data+=normalizedInput;
        }

        layer++;
    }

    if (singleOutput) {
        Neuron* node = mOutputNodes.at(0);
        string layerInput;
        for(unsigned int wi = 0; wi < node->getWeights().size(); wi++) {
            // If first hidden layer

            string withWeight = "layer" + to_string(layer - 1) + "x" + to_string(wi)  + " * " + to_string(node->getWeights().at(wi));
            layerInput += par(withWeight);

            if (wi < node->getWeights().size() - 1) {
                layerInput += " + ";
            }
        }

        data+="  return pw >= " + par(par(par(par(layerInput) + " + " + to_string(node->getBias()))) + " * " + to_string(node->getVariance())) + " + " +  to_string(node->getMedian()) + ";";

    } else {
        data+="  vector<float> outputs;\n";
        for (auto node : mOutputNodes) {
            string layerInput;
            for(unsigned int wi = 0; wi < node->getWeights().size(); wi++) {
                // If first hidden layer

                string withWeight = "layer" + to_string(layer - 1) + "x" + to_string(wi)  + " * " + to_string(node->getWeights().at(wi));
                layerInput += par(withWeight);

                if (wi < node->getWeights().size() - 1) {
                    layerInput += " + ";
                }
            }

            data+="  outputs.push_back" + par(par(par(par(par(layerInput) + " + " + to_string(node->getBias()))) + " * " + to_string(node->getVariance())) + " + " +  to_string(node->getMedian())) + ";\n";
        }

        data+="  return outputs;";
    }

    data+="\n}";
    return data;
}

string MlModel::getActivationName(Neuron::Activation activation) {
    switch (activation) {
        case Neuron::Activation::SIGMOID:
            return "sigmoid";
        case Neuron::Activation::TANH:
            return "tanh";
        case Neuron::Activation::RELU:
            return "relu";
        case Neuron::Activation::LINEAR:
            return "";
        case Neuron::Activation::VNA:
            return "vna";
        default:
            return "";
    }
}

string MlModel::par(string input) {
    return "(" + input + ")";
}



void  MlModel::resetNetwork() {
    for (vector<Neuron*> layer : mTrainable) {
        for (Neuron* node : layer) {
            for (uint i = 0; i < node->getWeights().size(); i++) {
                node->setWeight(getInitialWeight(layer.size()), i);
                node->setBias(getInitialWeight(layer.size()));
                node->resetPending();
                node->resetRunningAverage();
            }
        }
    }
}

void MlModel::percussiveMaintenance() {
    for (const vector<Neuron*>& layer : mTrainable) {
        for (Neuron* node : layer) {
            for (unsigned int i = 0; i < node->getWeights().size(); i++) {
                node->setAverageGrad(i, 200.0);
            }

            srand(mSeed++);
            node->setAverageBias( 200.0);
        }
    }
}

void MlModel::computeNormalization() {
    computeNormalization(mTrainingData);
}

void MlModel::trainNetwork(int epochCount, MlModel *model) {
    model->mTrainingInProgress = true;
    for (int j = 0; j < epochCount; j++) {
        model->mTotalCost = 0.0;
        int batchCount = model->mTrainingData.size() / model->mMiniBatchSize;


        for (int b = 0; b < batchCount; b++) {
            auto first = model->mTrainingData.cbegin() + (b * model->mMiniBatchSize);
            auto last = model->mTrainingData.cbegin() + ((b + 1) * model->mMiniBatchSize);

            auto miniBatch = vector<pair<vector<double>, vector<double>>> (first, last);

            //computeNormalization(miniBatch);

            double lastCost = model->mTotalCost;
            for (const pair<vector<double>, vector<double>>& sample : miniBatch) {
                model->trainSingle(sample.first, sample.second);
                model->mTotalCost += model->getTotalCostNode()->compute() / model->mTrainingData.size();
            }

            model->getTotalCostNode()->applyPending();

            if (model->mTrainingCallback != nullptr) {
                model->mTrainingCallback();
            }
        }


        if (model->mShouldStop) {
            model->mShouldStop = false;
            model->mTrainingInProgress = false;
            return;
        }

        // Compute value
        if (!model->mJobs.empty()) {
            vector<vector<float>> jobOutputs;
            for (vector<float> inputs : model->mJobs) {
                int index = 0;
                for (float input : inputs) {
                    model->setInput(input, index);
                    index++;
                }

                model->compute();

                vector<float> outputs;
                for (int i = 0; i < model->mOutputs; i++) {
                    outputs.push_back(model->getOutputAt(i));
                }

                jobOutputs.push_back(outputs);
            }


            if (model->mJobsCallback != nullptr) {
                model->mJobsCallback(jobOutputs);
            }
            model->mJobs.clear();
        }
    }

    model->mTrainingInProgress = false;
}

void MlModel::computeAsync(vector<vector<float>> input, function<void(vector<vector<float>>)> function) {
    mJobs = std::move(input);
    mJobsCallback = std::move(function);
}

void MlModel::setTrainingCallback(function<void()> callback) {
    mTrainingCallback = callback;
}

void MlModel::computeNormalization(const vector<pair<vector<double>, vector<double>>>& data) {

    int i = 0;
    for (Neuron* node : mInputNodes) {
        double minIn = 1e8;
        double maxIn = -1e8;
        double mean = 0;
        double variance = 0;

        for (pair<vector<double>, vector<double>> sample : data) {
            minIn = min(sample.first.at(i), minIn);
            maxIn = max(sample.first.at(i), maxIn);
            mean += sample.first.at(i) / data.size();
        }


        for (pair<vector<double>, vector<double>> sample : data) {
            variance += pow(sample.first.at(i) - mean, 2) / data.size();
        }
        variance = sqrt(variance);

        node->setMedian(((maxIn - minIn) / 2) + minIn);
        node->setVariance(variance);
        i++;
    }

    i = 0;
    for (Neuron* node : mOutputNodes) {
        double minIn = 1e8;
        double maxIn = -1e8;
        double mean = 0;
        double variance = 0;

        for (pair<vector<double>, vector<double>> sample : data) {
            minIn = min(sample.second.at(i), minIn);
            maxIn = max(sample.second.at(i), maxIn);
            mean += sample.second.at(i) / data.size();
        }

        for (pair<vector<double>, vector<double>> sample : data) {
            variance += pow(sample.second.at(i) - mean, 2) / data.size();
        }

        variance = sqrt(variance);

        node->setMedian(((maxIn - minIn) / 2) + minIn);
        node->setVariance(variance);
        i++;
    }
}

void MlModel::setMiniBatchSize(int size) {
    mMiniBatchSize = size;
}

void MlModel::setTrainingData(vector<pair<vector<double>, vector<double>>> data) {
    mTrainingData = std::move(data);
    // vna model
    auto rng = default_random_engine{};
    shuffle(mTrainingData.begin(), mTrainingData.end(), rng);
}

double MlModel::getInitialWeight(double height) {
    double var = 5.0 / height;
    srand(mSeed++);
    double number = var * ((double) rand() / (double) RAND_MAX)  - (var / 2.0);
    return number;
}

void MlModel::trainSingle(double input, double expectedOutput)  {
    vector<double> in = {input};

    vector<double> out = {expectedOutput};
    trainSingle(in, out);
}

void MlModel::trainSingle(const vector<double>& input, const vector<double>& expectedOutput) {
    // Input and output vectors should be the same length as
    // the number of input and output nodes.

    int count = 0;
    for (double in : input) {
        mInputNodes.at(count)->setValue((in - mInputNodes.at(count)->getMedian()) / mInputNodes.at(count)->getVariance());
        count++;
    }

    int countOut = 0;
    for (double out : expectedOutput) {


//        double number = mDist(mGenerator);
//        out += number;
        double expected = (out - mOutputNodes.at(countOut)->getMedian()) / mOutputNodes.at(countOut)->getVariance();
        mCostNodes.at(countOut)->setExpectedValue(expected);
        countOut++;
    }

    // Run inference
    computeAll();

    float step = mStep;

    for (vector<Neuron*> layer : mTrainable) {
        for (Neuron* node : layer) {


            if (mOptimizer == ADAGRAD) {
                double db = node->computeDEDB();

                node->setAverageBias(node->getAverageBias() + db * db);
                node->updatePendingBias(-(step / (sqrt(node->getAverageBias()) + 1e-6)) * db);

                for (unsigned int wi = 0; wi < node->getWeights().size(); wi++) {

                    double dw = node->computeDEDW(wi) + (pow(node->getWeights().at(wi), 1.0)) / mTrainingData.size();
                    node->setAverageGrad(wi, node->getAverageGrads().at(wi) + dw * dw);
                    node->updatePendingWeight(-(step / (sqrt(node->getAverageGrads().at(wi)) + 1e-6)) * dw, wi);
                }
            } else if (mOptimizer == RMSPROP) {
                double R = 0.95;
                double e = 1.e-2;
                // Update Bias
                double db = node->computeDEDB();
                node->setAverageBias((R) * node->getAverageBias() + ((1.0 - R) * pow(db, 2)));
                node->updatePendingBias(((-step * db) / sqrt(node->getAverageBias()  + e)));

                // Update weights
                for (unsigned int wi = 0; wi < node->getWeights().size(); wi++) {
                    double dw = node->computeDEDW(wi) + (pow(node->getWeights().at(wi), 2.0) * 0.1) / mTrainingData.size();
                    node->setAverageGrad(wi, (R) * node->getAverageGrads().at(wi) + ((1.0 - R) * pow(dw, 2)));
                    node->updatePendingWeight(( (-(step * dw) / sqrt(node->getAverageGrads().at(wi) + e)) ), wi);
                }
            } else if (mOptimizer == MOMENTUM) {
                double R = 0.9;
                // Update Bias
                double db = node->computeDEDB();
                node->setAverageBias((R) * node->getAverageBias() + ((1.0 - R) * db));
                node->updatePendingBias(((-step * node->getAverageBias())));

                // Update weights
                for (unsigned int wi = 0; wi < node->getWeights().size(); wi++) {
                    double dw = node->computeDEDW(wi);
                    node->setAverageGrad(wi, R * node->getAverageGrads().at(wi) + ((1.0 - R) * dw));
                    node->updatePendingWeight(( (-(step * node->getAverageGrads().at(wi)))), wi);
                }
            } else if (mOptimizer == GD) {
                double db = node->computeDEDB();

                node->setAverageBias(node->getAverageBias() + db * db);
                node->updatePendingBias(-(step * (db)));

                for (unsigned int wi = 0; wi < node->getWeights().size(); wi++) {

                    double dw = node->computeDEDW(wi) + (pow(node->getWeights().at(wi), 1.0)) / mTrainingData.size();
                    node->setAverageGrad(wi, node->getAverageGrads().at(wi) + dw * dw);
                    node->updatePendingWeight(-step  * (dw ), wi);
                }
            }
        }
    }

}

void MlModel::setActivationFunction(Neuron::Activation activation) {
    int index = 0;
    for (const vector<Neuron*>& layer : mTrainable) {
        if (index != 0) {
            for (Neuron *node : layer) {
                node->setType(activation);
            }
        }

        index++;
    }
}

Neuron* MlModel::getNodeAt(int w, int h) {
    int x = getWidth() - w - 1;
    return mLayers.at(x).at(h);
}

void MlModel::compute() {
    for (Neuron* node : mOutputNodes) {
        node->compute();
    }
}

void MlModel::setInput(double input, int index) {
    return mInputNodes.at(index)->setValue((input - mInputNodes.at(index)->getMedian()) / mInputNodes.at(index)->getVariance());
}

double MlModel::getOutputAt(int index) {
    return (mOutputNodes.at(index)->compute() * mOutputNodes.at(index)->getVariance()) + mOutputNodes.at(index)->getMedian();
}

int MlModel::getWidth() {
    return mLayers.size();
}

int MlModel::getLayerHeight(int index) {
    return mLayers.at(getWidth() - index - 1).size();
}

void MlModel::computeAll() const {
    mTotalCostNode->invalidate();
}

Neuron *MlModel::getCostNode(int index) {
    return mCostNodes.at(index);
}

Neuron* MlModel::getTotalCostNode() {
    return mTotalCostNode;
}

void MlModel::setStepSize(double value) {
    mStep = value;
}

void MlModel::setOptimizer(Optimizer mode) {
    mOptimizer = mode;
}

vector<vector<Neuron *>> MlModel::getAllLayers() {
    return mLayers;
}

double MlModel::getTotalCost() {
    return mTotalCost;
}

int MlModel::getInputCount() {
    return mInputs;
}

double MlModel::getStepSize() {
    return mStep;
}

int MlModel::getMiniBatchSize() {
    return mMiniBatchSize;
}

MlModel::Optimizer MlModel::getOptimizer() {
    return mOptimizer;
}

string MlModel::getTrainingPath() {
    return mTrainingPath;
}

vector<pair<vector<double>, vector<double>>>
MlModel::getTrainingData() {
    return mTrainingData;
}

void MlModel::trainNetworkAsync(int epoch) {
    mTrainingThread = thread(trainNetwork, epoch, this);
    mTrainingThread.detach();
}
