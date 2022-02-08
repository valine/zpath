//
// Created by lukas on 1/24/22.
//

#ifndef ZPATH_DATAIMPORTUTIL_H
#define ZPATH_DATAIMPORTUTIL_H

#include "json/json.hpp"
#include <iostream>
#include <fstream>
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

    static json parseJsonFromFile(string path) {

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
    static void storeJson(json data) {
        vector<float> list = data["BrainSenseTimeDomain"][0]["TimeDomainData"];
        for (float point : list) {
            cout << to_string(point) << endl;
        }
    }

    vector<float> getDataAtIndex(int index) {

    }

    void testJson() {
       // json j;
        //j["hello"] = "temp";
       /// cout << j.dump() << endl;
    }

private:

    map<string, vector<float>> mDataMap;
    map<int, string> mDataIndices;

    DataStore() {
    }
};


#endif //ZPATH_DATAIMPORTUTIL_H
