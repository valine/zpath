//
// Created by lukas on 4/16/20.
//

#include "neuralcore/cubeneuron.h"

double CubeNeuron::activation(double in) {
    double val = in * 1.0;
    if (in > 0.0) {
        val = pow(in, 3);
    }

    if (mInvert)  {
        val *= -1;
    }
    return val;
}

double CubeNeuron::activationPrime(double in, double regWeight) {
    double prime = 1.0;
    if (in > 0.0) {
        prime = 3 * pow(in, 2);
    }

    if (mInvert)  {
        prime *= -1;
    }
    return prime;
}
