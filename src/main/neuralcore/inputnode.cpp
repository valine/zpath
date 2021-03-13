//
// Created by lukas on 4/12/20.
//

#include "neuralcore/inputnode.h"

double InputNode::activation(double) {
    return mValue;
}

double InputNode::activationPrime(double, double regWeight) {
    return 1.0;
}

InputNode::InputNode() {

}

double InputNode::compute() {
    return mValue;
}
