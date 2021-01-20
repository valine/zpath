//
// Created by lukas on 1/17/21.
//

#include "utils/casutil.h"
#include <giac/config.h>
#include <giac/giac.h>

#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>
using namespace std;
using namespace giac;


void CasUtil::testCompute() {

    context ct;
    gen e("laplace(sin(x))", &ct);
    e = eval(e, 1, &ct);

    string out;
    stringstream stream(out);
    stream << e << endl;

    cout << e << endl;

    string testString = "3 + 4(10+2*2)+1";
    testString.erase(std::remove_if(testString.begin(), testString.end(), ::isspace), testString.end());

    char testArray[testString.length() + 1];
    strcpy(testArray, testString.c_str());
    char *copy = strdup(testArray);

    const char* delim = " +-*/()";
    char* afterSplit = strtok(testArray, delim);

    while (afterSplit != nullptr) {

        char d = copy[afterSplit - testArray + strlen(afterSplit)];
        cout << "t:" << afterSplit << endl;
        cout << "d:" << d << endl;



        afterSplit = strtok(nullptr, delim);
    }


    /// symbols test
}
