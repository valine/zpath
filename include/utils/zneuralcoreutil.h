//
// Created by lukas on 1/18/21.
//

#ifndef ZPATH_ZNEURAL_CORE_UTIL_H
#define ZPATH_ZNEURAL_CORE_UTIL_H

#include <utils/znodeutil.h>
#include <string>

class ZNodeUtil;
using namespace std;

class ZNeuralCoreUtil {

private:

public:
    static ZNeuralCoreUtil& get(){
        static ZNeuralCoreUtil instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }


};

#endif //ZPATH_ZNEURAL_CORE_UTIL_H
