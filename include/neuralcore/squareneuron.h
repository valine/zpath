//
// Created by lukas on 4/16/20.
//

#ifndef NEURALCORE_SQUARENEURON_H
#define NEURALCORE_SQUARENEURON_H


#include <neuralcore/neuron.h>

class SquareNeuron : public Neuron {

    double activation(double in) override;
    double activationPrime(double in, double regWeight) override;

};


#endif //NEURALCORE_SQUARENEURON_H
