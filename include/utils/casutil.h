//
// Created by lukas on 1/17/21.
//

#ifndef ZPATH_CASUTIL_H
#define ZPATH_CASUTIL_H

#include <iostream>
#include <vector>

using namespace std;

class CasUtil {


public:

    void testCompute();

    static CasUtil& get(){
        static CasUtil instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

};


#endif //ZPATH_CASUTIL_H
