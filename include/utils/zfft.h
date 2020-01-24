
#ifndef ZPATH_ZFFTUTIL_H
#define ZPATH_ZFFTUTIL_H

#include <string>
#include <complex.h>
#include "ui/z3dview.h"

using namespace std;

class ZFFt {

public:
    void calculate(double* data, unsigned long nn);

    void convolve(const vector<std::complex<double>> &xvec, const vector<std::complex<double>> &yvec,
                  vector<std::complex<double>> &outvec);

    void transformBluestein(vector<std::complex<double>> &vec);

    void transformRadix2(vector<std::complex<double>> &vec);

    void inverseTransform(vector<std::complex<double>> &vec);

    void transform(vector<std::complex<double>> &vec);
};


#endif //ZPATH_ZFFTUTIL_H
