//
// Created by lukas on 4/3/20.
//

#include <cmath>
#include "neuralcore/sigmoidneuron.h"

SigmoidNeuron::SigmoidNeuron() = default;

double SigmoidNeuron::activation(double x) {
    return (1.0 / (1.0 + exp(-x)));
}

double SigmoidNeuron::activationPrime(double x, double regWeight) {
    return activation(x) * (1.0 - activation(x));
}


