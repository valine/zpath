//
// Created by lukas on 4/10/20.
//

#ifndef NEURALCORE_NEURON_H
#define NEURALCORE_NEURON_H


static const double DEFAULT_BIAS = 0.01;

#include "neuron.h"
#include <vector>
#include <string>

using namespace std;
class Neuron {



public:
    enum Activation {
        LINEAR,
        RELU,
        SIGMOID,
        TANH,
        VNA

    };
    Neuron();
    Neuron(double value);
    virtual double compute();
    double getValue();
    double mValue;
    string getName();

    double mGrad = 1.0;

    void setType(Activation type);
    Activation getType();

    double getExpected();
    void setExpectedValue(double value);

    virtual double activation(double input);
    virtual double activationPrime(double input, double regWeight);


    void setValue(double value);
    void addParent(Neuron *node, double weight);
    void addChild(Neuron* node);
    void setName(string name);
    Neuron* getChild(int index);

    void setWeight(double weight, int index);

    void updatePendingWeight(double delta, int index);
    void resetPendingWeights();

    double getBias();
    void setBias(double bias);

    void updatePendingBias(double delta);
    void resetPendingBias();

    void applyPending();

    double computeDWDETest(int weightIndex, Neuron *costNode);
    double computeDEDW(int weightIndex);
    double computeDEDA(double weightSum, double regWeight);

    double computeWeightedSum() const;
    vector<double> getWeights();
    void invalidate();

    Neuron *getParent(int index);
    vector<Neuron*> getParents();

    double computeDEDB();
    double computeDBDETest(Neuron *costNode);

    void resetPending();

    void setInvert(bool invert);
    bool getInvert();
    bool mInvert = false;

    void setMedian(double median);
    double getMedian();

    void setVariance(double var);
    double getVariance();

    vector<double> getAverageGrads();
    double getAverageBias();

    void setAverageBias(double value);

    void setAverageGrad(int index);

    void setAverageGrad(int index, double value);

    void resetRunningAverage();

private:
    vector<double> mWeights;
    vector<double> mPendingWeights;
    vector<double> mAverageGradient;

    double mBias = DEFAULT_BIAS;
    double mPendingBias = 0.0;
    double mAverageBias = mGrad;


    double mGamma = 1.0;
    double mPendingGamma = 1.0;

    vector<Neuron*> mParents;
    vector<Neuron*> mChildren;

    string mName;

    double mExpectedValue;
    bool mValueSet = false;

    double mInputMedian = 0.0;
    double mInputVariance = 1.0;

    Activation mType = LINEAR;

};


#endif //NEURALCORE_NEURON_H
