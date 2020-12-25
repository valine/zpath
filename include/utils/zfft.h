#ifndef ZPATH_ZFFTUTIL_H
#define ZPATH_ZFFTUTIL_H

#include <string>
#include <complex.h>
#include <vector>

using namespace std;

class ZFFt {

public:
    static void calculate(float* data, unsigned long nn);

    static void convolve(const vector<std::complex<float>> &xvec, const vector<std::complex<float>> &yvec,
                  vector<std::complex<float>> &outvec);

    static void transformBluestein(vector<std::complex<float>> &vec);

    static void transformRadix2(vector<std::complex<float>> &vec);

    static void inverseTransform(vector<std::complex<float>> &vec);

    static void transform(vector<std::complex<float>> &vec);
};


#endif //ZPATH_ZFFTUTIL_H
