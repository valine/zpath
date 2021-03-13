//
// Created by lukas on 4/29/20.
//

#ifndef NEURALCORE_CSVUTILS_H
#define NEURALCORE_CSVUTILS_H

#include <vector>
#include <string>
#include "mlmodel.h"

using namespace std;

class CsvUtils {

public:
    static vector<pair<vector<double>, vector<double>>> loadCsv(string path);
    static void saveCsv(const vector<pair<vector<double>, vector<double>>>& data, string path);
    static vector<string> split(const string &s, char delim);

    static void saveModel(MlModel *model, string filename);

    static string getDateTime();

    static MlModel *loadModel(const string &fileName, string testName);

    static void incCounter();

    static int getCount();

    vector<string> getModels();


    static bool fileExists(const string &name);
};


#endif //NEURALCORE_CSVUTILS_H
