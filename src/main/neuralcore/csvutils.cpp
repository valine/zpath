#include <utility>

//
// Created by lukas on 4/29/20.
//

#include <fstream>
#include <sstream>
#include <neuralcore/mlmodel.h>
#include "neuralcore/csvutils.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <utils/zsettingsstore.h>

vector<string> CsvUtils::split(const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }
    return result;
}

string CsvUtils::getDateTime() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M",timeinfo);
    std::string str(buffer);
    return str;
}

bool CsvUtils::fileExists (const std::string& name) {
    ifstream f(name.c_str());
    return f.good();
}

MlModel * CsvUtils::loadModel(const string &fileName, string testName) {
    string path = ZSettings::get().getResourcePath() + "resources/models/" + fileName + ".txt";
    std::ifstream t(path);
    std::string dataString;
    t.seekg(0, std::ios::end);
    dataString.reserve(t.tellg());
    t.seekg(0, std::ios::beg);
    dataString.assign((std::istreambuf_iterator<char>(t)),
                      std::istreambuf_iterator<char>());

    // Lines has all the data
    vector<string> lines = split(dataString, '\n');

    // First line - Model hyper parameters
    vector<string> metaData = split(lines.at(0), ',');
    double stepSize = stof(metaData.at(0));
    int miniBatchSize = stoi(metaData.at(1));
    int optimizer = stoi(metaData.at(2));

    // Second Line - Layer heights
    vector<string> layersHeights = split(lines.at(1), ',');
    int outputs = stoi(layersHeights.at(2));
    int inputs = stoi(layersHeights.at(layersHeights.size() - 1));
    int width = layersHeights.size() - 4;

    vector<int> hiddenHeights;
    for (unsigned int i = 3; i < layersHeights.size() - 1; i++) {
        hiddenHeights.push_back(stoi(layersHeights.at(i)));
    }
    std::reverse(hiddenHeights.begin(),hiddenHeights.end());

    // Third line - Layer activation types, eg. Sigmoid, Relu, Linear
    vector<string> types = split(lines.at(2), ',');
    std::reverse(types.begin(),types.end());
    vector<Neuron::Activation> activationsTypes;
    for (unsigned int i = 1; i < hiddenHeights.size() + 1; i++) {
        activationsTypes.push_back(Neuron::Activation(stoi(types.at(i))));
    }

    // After the third line we have enough data to initialize the model,
    // but we still need the trained parameter values
    MlModel* model = new MlModel(width, hiddenHeights, inputs,
            outputs, miniBatchSize, stepSize,
            MlModel::Optimizer(optimizer), activationsTypes);

    // We need to know how many nodes there are to parse weights
    int nodeCount = inputs + outputs;
    for (unsigned int hiddenHeight : hiddenHeights) {
        nodeCount+=hiddenHeight;
    }

    // Forth line - Neuron parameters
    int nodeStartingLine = 3;

    for (const vector<Neuron*>& layer : model->getAllLayers()) {
        for (Neuron* node : layer) {
            vector<string> nodeData = split(lines.at(nodeStartingLine), ',');
            string name = nodeData.at(0);
            double median = stod(nodeData.at(1));
            double variance = stod(nodeData.at(2));
            double bias = stod(nodeData.at(3));
            double averageBias = stod(nodeData.at(4));

            node->setMedian(median);
            node->setVariance(variance);
            node->setBias(bias);
            node->setAverageBias(averageBias);


            vector<string> weightStrings = split(lines.at(nodeStartingLine + 1), ',');
            for (unsigned int i = 0; i < node->getWeights().size(); i++) {
                node->setWeight(stod(weightStrings.at(i)), i);
            }
            node->resetPending();

            vector<string> averageWeightStrings = split(lines.at(nodeStartingLine + 2), ',');
            for (unsigned int i = 0; i < node->getAverageGrads().size(); i++) {
                node->setAverageGrad(i, stod(averageWeightStrings.at(i)));
            }

            nodeStartingLine+=3;
        }
    }

    vector<pair<vector<double>, vector<double>>> trainingData = CsvUtils::loadCsv(std::move(testName));
    int trainingSize = trainingData.size();
    model->setTrainingData(trainingData);
    model->setMiniBatchSize(std::min(model->getMiniBatchSize(), trainingSize));
    return model;
}

vector<string> CsvUtils::getModels() {
//    string path = FieldUtils::get().getResourcePath() + "resources/models/";
//    for (const auto & entry : fs::directory_iterator(path))
//        std::cout << entry.path() << std::endl;
    return vector<string>();
}

void CsvUtils::incCounter() {
    string path = ZSettings::get().getResourcePath() + "resources/models/count.txt";

    std::ifstream t(path);
    std::string dataString;

    t.seekg(0, std::ios::end);
    dataString.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    dataString.assign((std::istreambuf_iterator<char>(t)),
                      std::istreambuf_iterator<char>());

    int count = stoi(dataString);

    std::ofstream out(path);
    out << to_string(count+1);
    out.close();
}

int CsvUtils::getCount() {
    string path = ZSettings::get().getResourcePath() + "resources/models/count.txt";

    std::ifstream t(path);
    std::string dataString;

    t.seekg(0, std::ios::end);
    dataString.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    dataString.assign((std::istreambuf_iterator<char>(t)),
                      std::istreambuf_iterator<char>());

    int count = stoi(dataString);
    return count;
}

void CsvUtils::saveModel(MlModel* model, string fileName) {
    string path = ZSettings::get().getResourcePath() + "resources/models/" + fileName + to_string(getCount()) + ".txt";
    incCounter();
    string dataString;

    dataString += to_string(model->getStepSize()) + ",";
    dataString += to_string(model->getMiniBatchSize()) + ",";
    dataString += to_string(model->getOptimizer()) + ",";
    dataString += model->getTrainingPath() + "\n";

    unsigned int index = 0;
    for (const vector<Neuron*>& layer : model->getAllLayers()) {
        dataString += to_string(layer.size());
        if (index < model->getAllLayers().size() - 1) {
            dataString += ",";
        }
        index++;
    }
    dataString+="\n";

    for (const vector<Neuron*>& layer : model->getAllLayers()) {
        dataString += to_string(layer.at(0)->getType()) + ",";
    }
    dataString+="\n";

    for (const vector<Neuron*>& layer : model->getAllLayers()) {
        for (Neuron* node : layer) {
            dataString += node->getName() + ",";
            dataString += to_string(node->getMedian()) + ",";
            dataString += to_string(node->getVariance()) + ",";
            dataString += to_string(node->getBias()) + ",";
            dataString += to_string(node->getAverageBias()) + "\n";

            for (double weight : node->getWeights()) {
                dataString += to_string(weight) + ", ";
            }
            dataString += "\n";

            for (double aweight : node->getAverageGrads()) {
                dataString += to_string(aweight) + ", ";
            }
            dataString += "\n";
        }
    }

    std::ofstream out(path);
    out << dataString;
    out.close();
}

vector<pair<vector<double>, vector<double>>> CsvUtils::loadCsv(string fileName) {
    string path = ZSettings::get().getResourcePath() + "resources/training/" + fileName + ".csv";

    std::ifstream t(path);
    std::string dataString;

    t.seekg(0, std::ios::end);
    dataString.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    dataString.assign((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

    vector<pair<vector<double>, vector<double>>> data;

    vector<string> samples = split(dataString, '\n');
    int seed = 0;
    double var = 0;
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0,var);

    auto rng = default_random_engine{};
    shuffle(samples.begin(), samples.end(), rng);
    int anodeCount = 0;
    int cathodeCount = 0;

    int anodeCount2 = 0;
    int cathodeCount2 = 0;
    int i = 0;
    for (const string& sample : samples) {

        vector<string> dataPoints = split(sample, ',');

        string inputString = dataPoints.at(0);
        string outputString = dataPoints.at(1);

        vector<string> inputStrings = split(inputString, ':');
        vector<string> outputStrings = split(outputString, ':');

        vector<double> inputs;
        vector<double> outputs;

        double inputZero =  stof(inputStrings.at(0));
        inputs.push_back(stof(inputStrings.at(1)));
        inputs.push_back(stof(inputStrings.at(2)) * 0.6);
        inputs.push_back(stof(inputStrings.at(3)) * 1.0);

        if (i++ % 8 != 0) {
            continue;
        }

//        if (inputZero < 0) {
//            anodeCount++;
//            if (anodeCount % 12 != 2) {
//                continue;
//            }
//        } else {
//            cathodeCount++;
//            if (cathodeCount % 3 != 0) {
//                continue;
//            }
//        }



        for(const string& singleOutput : outputStrings) {
            double output = stof(singleOutput);
            double number = distribution(generator);
            outputs.push_back(output);
            cout << number << endl;
        }

//        if (outputs.at(0) > 450) {
//            continue;
//        }


        if (inputZero < 0) {
            anodeCount2++;
        } else {
            cathodeCount2++;
        }

        if (inputZero < 0) {
            // Anode
            if (outputs.at(0) > 280) {
                outputs.at(0) += 75.0;
            } else {
                outputs.at(0) += 18.0;
            }

        } else {
            // Cathode
            if (outputs.at(0) > 80) {
                outputs.at(0) -= 22.0;
            }
        }
        pair<vector<double>, vector<double>> dataPair(inputs, outputs);



//        if (outputs.at(0) < 40) {
//            data.push_back(dataPair);
//            data.push_back(dataPair);
//        }
        data.push_back(dataPair);
    }

    cout << "cathode: " << to_string(cathodeCount) << " anode: " << to_string(anodeCount) << endl;
    cout << "cathode: " << to_string(cathodeCount2) << " anode: " << to_string(anodeCount2) << endl;

    return data;
}

void CsvUtils::saveCsv(const vector<pair<vector<double>, vector<double>>>& data, string fileName) {
    string path = ZSettings::get().getResourcePath() + "resources/training/" + fileName + ".csv";

    string dataString;
    for (const pair<vector<double>, vector<double>>& dataPair : data) {
        for (double in : dataPair.first) {
            dataString += to_string(in);
            dataString += ":";
        }

        dataString += ", ";

        for (double out : dataPair.second) {
            dataString += to_string(out);
            dataString += ":";
        }

        dataString += "\n";
    }

    std::ofstream out(path);
    out << dataString;
    out.close();
}
