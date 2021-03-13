//
// Created by lukas on 4/16/20.
//

#ifndef NEURALCORE_CUBENEURON_H
#define NEURALCORE_CUBENEURON_H


#include <neuralcore/neuron.h>
#include <cmath>
class CubeNeuron : public Neuron {
    double activation(double in) override;
    double activationPrime(double in, double regWeight) override;
};


#endif //NEURALCORE_CUBENEURON_H
