//
// Created by lukas on 4/3/20.
//

#ifndef NEURALCORE_SIGMOIDNEURON_H
#define NEURALCORE_SIGMOIDNEURON_H

#include <vector>
#include <string>
#include "neuron.h"

using namespace std;

class SigmoidNeuron : public Neuron {

public:

    SigmoidNeuron();
    double activation(double);
    double activationPrime(double, double regWeight);
};


#endif //NEURALCORE_SIGMOIDNEURON_H
