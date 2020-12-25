#ifndef ZPATH_ZFFTUTIL_H
#define ZPATH_ZFFTUTIL_H

#include <string>
#include <complex.h>
#include <vector>

using namespace std;

class ZFFt {

public:
    static void calculate(double* data, unsigned long nn);

    static void convolve(const vector<std::complex<double>> &xvec, const vector<std::complex<double>> &yvec,
                  vector<std::complex<double>> &outvec);

    static void transformBluestein(vector<std::complex<double>> &vec);

    static void transformRadix2(vector<std::complex<double>> &vec);

    static void inverseTransform(vector<std::complex<double>> &vec);

    static void transform(vector<std::complex<double>> &vec);
};


#endif //ZPATH_ZFFTUTIL_H
