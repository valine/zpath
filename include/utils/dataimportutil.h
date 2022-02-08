//
// Created by lukas on 1/24/22.
//

#ifndef ZPATH_DATAIMPORTUTIL_H
#define ZPATH_DATAIMPORTUTIL_H

#include <string>
#include "json/json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
using namespace nlohmann;

class DataStore {
public:
    static DataStore& getInstance(){
        static DataStore instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    static DataStore& get(){
        return getInstance();
    }

    json parseJsonFromFile(string path) {
        std::ifstream t(path);
        std::string dataString;
        t.seekg(0, std::ios::end);
        dataString.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        dataString.assign((std::istreambuf_iterator<char>(t)),
                          std::istreambuf_iterator<char>());

        json j = json::parse(dataString);

        return j;

    }

    /**
     * Parse json to float list and store
     * @param data Raw json data
     */
    void storeJson(json data, string path) {
        mDataList.push_back(path);
        vector<float> list = data["BrainSenseTimeDomain"][0]["TimeDomainData"];
        mDataMap.insert({path, list});
    }

    float getDataAtIndex(unsigned int fileIndex, float x) {
        if (fileIndex != -1 && mDataList.size() > fileIndex) {
            string path = mDataList.at(fileIndex);
            if (mDataMap.count(path) > 0) {
                vector<float> points = mDataMap.at(path);
                float size = points.size() - 1;
                float leftX = std::min(size, std::max(0.0f, floor(x)));
                float rightX = std::min(size, std::max(0.0f, ceil(x)));
                float factor = x - floor(x);

                float leftY = mDataMap.at(path).at((int) leftX);
                float rightY = mDataMap.at(path).at((int) rightX);

                return leftY + ((rightY - leftY) * factor);
            }
        }
        return 0;
    }

    vector<string> getFileList() {
        return mDataList;
    }

    vector<string> getFileNameList() {
        vector<string> fileNameList;
        for (string path : mDataList) {
            vector<string> tokens = split(path, '/');
            fileNameList.push_back(tokens.at(tokens.size() - 1));
        }

        return fileNameList;
    }

    vector<string> split(const string &s, char delim) {
        char escape = '\\';
        vector<string> result;
        stringstream ss (s);
        string item;

        string last = "";
        while (getline(ss, item, delim)) {

            // Delimiter was escaped
            if (!last.empty() && !result.empty() &&
                last.at(last.size() - 1) == escape) {

                last = item;
                int rsize = result.size() - 1;
                result.at(rsize).erase( result.at(rsize).size() - 1);
                result.at(result.size() - 1) += delim + item;
            } else {
                result.push_back(item);
                last = item;
            }

        }
        return result;
    }

    void testJson() {
       // json j;
        //j["hello"] = "temp";
       /// cout << j.dump() << endl;
    }

private:

    map<string, vector<float>> mDataMap;
    vector<string> mDataList;
    DataStore() {}
};


#endif //ZPATH_DATAIMPORTUTIL_H
