//
// Created by lukas on 4/15/20.
//

#include "neuralcore/reluneuron.h"

double ReluNeuron::activationPrime(double in, double regWeight) {
    double prime = 0.3;
    if (in > 0.0) {
        prime = 1;
    }

    if (mInvert)  {
        prime *= -1;
    }
    return prime;
}

double ReluNeuron::activation(double in) {
    double val = in * 0.3;
    if (in > 0.0) {
        val = in;
    }

    if (mInvert)  {
        val *= -1;
    }
    return val;
}