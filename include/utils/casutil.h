//
// Created by lukas on 1/17/21.
//

#ifndef ZPATH_CASUTIL_H
#define ZPATH_CASUTIL_H
#include <giac/config.h>
#include <giac/giac.h>
#include <iostream>
#include <vector>


#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>
using namespace std;

class CasUtil {


public:

    void testCompute() {
        giac::context ct;
        giac::gen e("laplace(sin(x))", &ct);
        e = eval(e, 1, &ct);

        string out;
        stringstream stream(out);
        stream << e << endl;

        cout << e << endl;
    }

    static CasUtil& get(){
        static CasUtil instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

};


#endif //ZPATH_CASUTIL_H
