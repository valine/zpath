//
// Created by lukas on 1/17/21.
//

#include "utils/casutil.h"

#include <pari.h>

void CasUtil::testCompute() {
    pari_init(8000000,500000);
    GEN out = gp_read_str("");
    cout << "Hello world" << endl;
}
