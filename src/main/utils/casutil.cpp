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
    gen e("laplace(x^2)", &ct);
    e = eval(e, 1, &ct);
    cout << e << endl;

}
