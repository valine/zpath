//
// Created by lukas on 1/24/22.
//

#ifndef ZPATH_DATAIMPORTUTIL_H
#define ZPATH_DATAIMPORTUTIL_H

#include "json/json.hpp"
#include <iostream>
using namespace std;
using namespace nlohmann;

class DataImportUtil {
public:
    static DataImportUtil& getInstance(){
        static DataImportUtil instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    static DataImportUtil& get(){
        return getInstance();
    }

    void testJson() {
        json j;
        j["hello"] = "temp";
        cout << j.dump() << endl;
    }

private:
    DataImportUtil() {
    }
};


#endif //ZPATH_DATAIMPORTUTIL_H
