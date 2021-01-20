//
// Created by lukas on 1/17/21.
//

#include "utils/casutil.h"
#include <giac/config.h>
#include <giac/giac.h>

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
    char testArray[testString.length() + 1];
    strcpy(testArray, testString.c_str());

    const char* delim = " +-*/()";
    char* afterSplit = strtok(testArray, delim);

    while (afterSplit != nullptr) {
        printf ("%s\n", afterSplit);
        afterSplit = strtok(nullptr, delim);
    }


    /// symbols test
}
