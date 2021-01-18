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
}
