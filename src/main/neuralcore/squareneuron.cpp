//
// Created by lukas on 4/16/20.
//

#include "neuralcore/squareneuron.h"
#include <cmath>

double SquareNeuron::activation(double in) {
    double val = in * 1.0;
    if (in > 0.0) {
        val = pow(in, 2);
    }

    if (mInvert)  {
        val *= -1;
    }

    return val;
}

double SquareNeuron::activationPrime(double in, double regWeight) {
    double prime = 1.0;
    if (in > 0.0) {
        prime = 2 * in;
    }

    if (mInvert)  {
        prime *= -1;
    }

    return prime;

}

