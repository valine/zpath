//
// Created by lukas on 4/11/20.
//

#include <cmath>
#include "neuralcore/costneuron.h"

double CostNeuron::activation(double actual) {
    double expected = getExpected();
    return pow(actual - expected, 2);
}

double CostNeuron::activationPrime(double actual, double regWeight) {
    double expected = getExpected();
    return ((actual - expected) * 2);
}

CostNeuron::CostNeuron() = default;
