//
// Created by lukas on 4/10/20.
//

#include <cmath>
#include <neuralcore/neuron.h>
#include <iostream>

#include "neuralcore/neuron.h"

double Neuron::activation(double input) {
    if (mType == LINEAR) {
        return input;
    } else if (mType == RELU) {
        double val = input * 0.3;
        if (input > 0.0) {
            val = input;
        }

        if (mInvert)  {
            val *= -1;
        }
        return val;
    } else if (mType == SIGMOID) {
        return (1.0 / (1.0 + exp(-input)));
    } else if (mType == TANH) {
        return (2.0 / (1.0 + exp(-2 * input))) - 1.0;
    } else if (mType == VNA) {
        if (input > 0.0) {
            return -1.0 / (input + 1);
        } else {
            return 1.0 + (input * 0.3);
        }
    }

    return 0;
}

double Neuron::activationPrime(double input, double regWeight) {
    if (mType == LINEAR) {
        return 1;
    } else if (mType == RELU) {
        double prime = 0.3;
        if (input > 0.0) {
            prime = 1;
        }

        if (mInvert)  {
            prime *= -1;
        }
        return prime;
    } else if (mType == SIGMOID) {
        return activation(input) * (1.0 - activation(input));
    } else if (mType == TANH) {
        return 1.0 - pow(activation(input), 2.0);
    } else if (mType == VNA) {

        return 0;
//        if (input > 0.0) {
//            return 1.0 / (pow(input + 1, 2));
//        } else {
//            return 0.3;
//        }

    }

    return 0;
}

double Neuron::compute() {
    if (!mValueSet) {
        double weightedSum = computeWeightedSum();
        setValue(activation(weightedSum + mBias));
    }

    return mValue;
}

double Neuron::computeWeightedSum() const {
    double weightedSum = 0.0;

    int index = 0;
    for (Neuron *node : mParents) {
        weightedSum += node->compute() * mWeights.at(index);
        index++;
    }
    return weightedSum;
}

void Neuron::setValue(double value) {
    mValue = value;
    mValueSet = true;
    for (Neuron* node : mChildren) {
        node->invalidate();
    }
}

void Neuron::setWeight(double weight, int index) {
    mWeights.at(index) = weight;
    mValueSet = false;
    for (Neuron* node : mChildren) {
        node->invalidate();
    }
}

void Neuron::addParent(Neuron *node, double weight) {
    mParents.push_back(node);
    node->addChild(this);
    mWeights.push_back(weight);
    mAverageGradient.push_back(mGrad);
    mPendingWeights.push_back(weight);
}

void Neuron::addChild(Neuron *node) {
    mChildren.push_back(node);
}

vector<double> Neuron::getWeights() {
    return mWeights;
}

vector<double> Neuron::getAverageGrads() {
    return mAverageGradient;
}

void Neuron::setAverageGrad(int index, double value) {
    mAverageGradient.at(index) = value;
}

double Neuron::computeDWDETest(int weightIndex, Neuron *costNode) {
    double initialWeight = getWeights().at(weightIndex);
    double finalWeight = initialWeight + 0.001;
    double initialCost = costNode->compute();

    setWeight(finalWeight, weightIndex);
    double finalCost = costNode->compute();
    double derivative = (finalCost - initialCost) / (finalWeight - initialWeight);

    setWeight(initialWeight, weightIndex);
    return derivative;
}


double Neuron::computeDBDETest(Neuron *costNode) {
    double initialBias = getBias();
    double finalBias = initialBias + 0.001;
    double initialCost = costNode->compute();

    setBias(finalBias);
    double finalCost = costNode->compute();
    double derivative = (finalCost - initialCost) / (finalBias - initialBias);

    setBias(initialBias);
    return derivative;
}

/**
 * Compute the derivative of the error with respect to the weight at index
 * @param weightIndex
 * @return
 */
double Neuron::computeDEDW(int weightIndex) {
    double x = mParents.at(weightIndex)->compute();
    double derivative = activationPrime(computeWeightedSum() + mBias, 0) * x;
    double childDDXSum = 0.0;
    if (!mChildren.empty()) {
        for (Neuron *child : mChildren) {
            double weightSum = 0;
            int wIndex = 0;
            for (Neuron* parent : child->getParents()) {
                if (parent == this) {
                    weightSum += child->getWeights().at(wIndex);
                }
                wIndex++;
            }

            childDDXSum += child->computeDEDA(weightSum, mWeights.at(weightIndex));
        }
    } else {
        childDDXSum = 1.0;
    }

    derivative *= childDDXSum;
    return derivative;
}

/**
 * Compute the derivative of the error with respect to the activation
 * @param weightSum
 * @return
 */
double Neuron::computeDEDA(double weightSum, double regWeight) {
    double derivative = (activationPrime(computeWeightedSum() + mBias, regWeight) * weightSum);
    double childDDXSum = 0.0;
    if (!mChildren.empty()) {
        int index = 0;
        for (Neuron *child : mChildren) {

            double childWeightSum = 0;
            int wIndex = 0;
            for (Neuron* parent : child->getParents()) {
                if (parent == this) {
                    childWeightSum += child->getWeights().at(wIndex);
                }
                wIndex++;
            }
            childDDXSum += child->computeDEDA(childWeightSum, regWeight);
            index++;
        }
    } else {
        childDDXSum = 1.0;
    }


    derivative *= childDDXSum;
    return derivative;
}

/**
 * Compute the derivative of the cost function with respect to the bias.
 * @return
 */
double Neuron::computeDEDB() {
    double derivative = activationPrime(computeWeightedSum() + mBias, 0);
    double childDDXSum = 0.0;
    if (!mChildren.empty()) {
        for (Neuron *child : mChildren) {
            double childWeightSum = 0;
            int wIndex = 0;
            for (Neuron* parent : child->getParents()) {
                if (parent == this) {
                    childWeightSum += child->getWeights().at(wIndex);
                }
                wIndex++;
            }

            childDDXSum += child->computeDEDA(childWeightSum, 0);
        }
    } else {
        childDDXSum = 1.0;
    }

    derivative *= childDDXSum;
    return derivative;
}

void Neuron::setName(string name) {
    mName = name;
}

string Neuron::getName() {
    return mName;
}

void Neuron::invalidate() {
    mValueSet = false;
    for (Neuron* node : mChildren) {
        node->invalidate();
    }
}

Neuron* Neuron::getChild(int index) {
    return mChildren.at(index);
}

Neuron* Neuron::getParent(int index) {
    return mParents.at(index);
}

Neuron::Neuron() = default;

Neuron::Neuron(double value) {
    setValue(value);
}

double Neuron::getValue() {
    return mValue;
}

double Neuron::getExpected() {
    return mExpectedValue;
}

void Neuron::setExpectedValue(double value) {
    mExpectedValue = value;
    for (Neuron* node : mChildren) {
        node->invalidate();
    }
}

vector<Neuron *> Neuron::getParents() {
    return mParents;
}

void Neuron::setBias(double bias) {
    mBias = bias;
    mValueSet = false;
    for (Neuron* node : mChildren) {
        node->invalidate();
    }
}

double Neuron::getBias() {
    return mBias;
}

void Neuron::updatePendingWeight(double delta, int index) {
    mPendingWeights.at(index) = mPendingWeights.at(index) + delta;
}

void Neuron::resetPendingWeights() {
    for (unsigned int i = 0; i < mPendingWeights.size(); i++) {
        mPendingWeights.at(i) = mWeights.at(i);
    }

    for (Neuron* node : mParents) {
        node->resetPendingWeights();
    }
}

void Neuron::updatePendingBias(double delta) {
    mPendingBias += delta;
}

void Neuron::resetPendingBias() {
    mPendingBias = mBias;
    for (Neuron* node : mParents) {
        node->resetPendingBias();
    }
}

void Neuron::resetPending() {
    resetPendingWeights();
    resetPendingBias();
}

void Neuron::resetRunningAverage() {
    mAverageBias = mGrad;
    for (int i = 0; i < mAverageGradient.size(); i++) {
        mAverageGradient.at(i) = mGrad;
    }
}

void Neuron::applyPending() {
    mBias = mPendingBias;
    for (unsigned int i = 0; i < mWeights.size(); i++) {
        mWeights.at(i) = mPendingWeights.at(i);
    }

    for (Neuron* node : mParents) {
        node->applyPending();
    }

    resetPending();
}

void Neuron::setInvert(bool invert) {
    mInvert = invert;
}

bool Neuron::getInvert() {
    return mInvert;
}

void Neuron::setMedian(double median) {
    mInputMedian = median;
}

double Neuron::getMedian() {
    return mInputMedian;
}

void Neuron::setVariance(double var) {
    mInputVariance = var;
}

double Neuron::getVariance() {
    return mInputVariance;
}

double Neuron::getAverageBias() {
    return mAverageBias;
}

void Neuron::setAverageBias(double value) {
    mAverageBias = value;
}

Neuron::Activation Neuron::getType() {
    return mType;
}

void Neuron::setType(Activation type) {
    mType = type;
}
