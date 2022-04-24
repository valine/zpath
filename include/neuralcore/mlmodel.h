//
// Created by lukas on 4/6/20.
//

#ifndef NEURALCORE_MLMODEL_H
#define NEURALCORE_MLMODEL_H

#include <vector>
#include "neuron.h"
#include <random>
#include <functional>
#include <thread>

class MlModel {
public:

    enum Optimizer {
        GD,
        ADAGRAD,
        RMSPROP,
        MOMENTUM
    };
    MlModel(int width, vector<int> height, int inputs, int outputs, int miniBatchSize, double stepSize,
            Optimizer optimizer, vector<Neuron::Activation> mTypes);
    void setStepSize(double value);
    void compute();
    void computeAll() const;
    Neuron* getNodeAt(int w, int h);
    Neuron *getCostNode(int index);
    Neuron* getExpectedNode();

    int getWidth();
    int getLayerHeight(int index);

    Neuron *getTotalCostNode();

    void trainSingle(const vector<double>& input, const vector<double>& expectedOutput);
    void trainSingle(double input, double expectedOutput);

    double getOutputAt(int index);

    void setInput(double input, int index);

    void setTrainingData(vector<pair<vector<double>, vector<double>>> data);
    vector<pair<vector<double>, vector<double>>> getTrainingData();
    static void trainNetwork(int epochCount, MlModel *model);

    void resetNetwork();
    void setOptimizer(Optimizer mode);

    void percussiveMaintenance();
    vector<vector<Neuron*>> getAllLayers();
    double getTotalCost();

    double getStepSize();
    int getMiniBatchSize();
    Optimizer getOptimizer();
    int getInputCount();
    string getTrainingPath();
    string toFunctionString();

    void setMiniBatchSize(int size);
    void computeNormalization();

    void addNodeToLayer(int layer);

    void setTrainingCallback(std::function<void()> callback);
    void trainNetworkAsync(int epoch);

    void requestStopTraining() {
        mShouldStop = true;
    }

    bool getTrainingInProgress() {
        return mTrainingInProgress;
    }

    string par(string input);
    void setActivationFunction(Neuron::Activation activation);
    void computeAsync(vector<vector<float>> input, function<void(vector<vector<float>>)> function);

    vector<Neuron*> mInputNodes;
    vector<Neuron*> mOutputNodes;
    vector<Neuron*> mCostNodes;
    Neuron* mTotalCostNode;
    vector<vector<Neuron*>> mLayers;
    vector<vector<Neuron*>> mTrainable;
    string mTrainingPath;

    string getActivationName(Neuron::Activation activation);

    vector<int> mHiddenHeight;

    int getOutputCount();

private:
    int mInputs = 1;
    int mOutputs = 1;
    int mHiddenWidth = 4;
    int mMiniBatchSize = 10;
    double mTotalCost = 0;

    bool mShouldStop = false;
    bool mTrainingInProgress = false;

    // Compute jobs to be done during training
    vector<vector<float>> mJobs;
    function<void(vector<vector<float>>)> mJobsCallback;


    std::default_random_engine mGenerator;
    std::normal_distribution<double> mDist;

    Optimizer mOptimizer = RMSPROP;

    double mStep = 0.001;
    int mSeed = time(0);

    vector<pair<vector<double>, vector<double>>> mTrainingData;
    function<void()> mTrainingCallback = nullptr;

    thread mTrainingThread;

    double getInitialWeight(double height);

    void computeNormalization(const vector<pair<vector<double>, vector<double>>>& data);

};


#endif //NEURALCORE_MLMODEL_H
