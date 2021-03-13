//
// Created by lukas on 4/11/20.
//

#ifndef NEURALCORE_COSTNEURON_H
#define NEURALCORE_COSTNEURON_H


#include <vector>
#include <string>
#include "neuron.h"

using namespace std;

class CostNeuron : public Neuron {

public:

    CostNeuron();
    double activation(double) override;
    double activationPrime(double, double regWeight) override;
};


#endif //NEURALCORE_COSTNEURON_H
