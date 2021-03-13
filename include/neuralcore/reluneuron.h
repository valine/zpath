//
// Created by lukas on 4/15/20.
//

#ifndef NEURALCORE_RELUNEURON_H
#define NEURALCORE_RELUNEURON_H


#include "neuron.h"

class ReluNeuron : public Neuron {

public:
    double activation(double in) override;
    double activationPrime(double in, double regWeight) override;
};


#endif //NEURALCORE_RELUNEURON_H
