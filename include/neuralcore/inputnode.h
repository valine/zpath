//
// Created by lukas on 4/12/20.
//

#ifndef NEURALCORE_INPUTNODE_H
#define NEURALCORE_INPUTNODE_H

#include <vector>
#include <string>
#include "neuron.h"

using namespace std;

class InputNode : public Neuron {

public:

    InputNode();
    double compute() override;
    double activation(double) override;
    double activationPrime(double, double regWeight) override;
};


#endif //NEURALCORE_INPUTNODE_H
