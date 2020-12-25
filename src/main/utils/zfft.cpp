#include <algorithm>
#include "utils/zfft.h"


using std::complex;
using std::size_t;
using std::vector;

// Private function prototypes
static size_t reverseBits(size_t x, int n);


void ZFFt::transform(vector<complex<float> > &vec) {
    size_t n = vec.size();
    if (n == 0)
        return;
    else if ((n & (n - 1)) == 0)  // Is power of 2
        transformRadix2(vec);
    else  // More complicated algorithm for arbitrary sizes
        transformBluestein(vec);
}


void ZFFt::inverseTransform(vector<complex<float> > &vec) {
    std::transform(vec.cbegin(), vec.cend(), vec.begin(),
                   static_cast<complex<float> (*)(const complex<float> &)>(std::conj));
    transform(vec);
    std::transform(vec.cbegin(), vec.cend(), vec.begin(),
                   static_cast<complex<float> (*)(const complex<float> &)>(std::conj));
}


void ZFFt::transformRadix2(vector<complex<float> > &vec) {
    // Length variables
    size_t n = vec.size();
    int levels = 0;  // Compute levels = floor(log2(n))
    for (size_t temp = n; temp > 1U; temp >>= 1)
        levels++;
    if (static_cast<size_t>(1U) << levels != n)
        throw std::domain_error("Length is not a power of 2");

    // Trignometric table
    vector<complex<float> > expTable(n / 2);
    for (size_t i = 0; i < n / 2; i++)
        expTable[i] = std::polar(1.0, -2 * M_PI * i / n);

    // Bit-reversed addressing permutation
    for (size_t i = 0; i < n; i++) {
        size_t j = reverseBits(i, levels);
        if (j > i)
            std::swap(vec[i], vec[j]);
    }

    // Cooley-Tukey decimation-in-time radix-2 FFT
    for (size_t size = 2; size <= n; size *= 2) {
        size_t halfsize = size / 2;
        size_t tablestep = n / size;
        for (size_t i = 0; i < n; i += size) {
            for (size_t j = i, k = 0; j < i + halfsize; j++, k += tablestep) {
                complex<float> temp = vec[j + halfsize] * expTable[k];
                vec[j + halfsize] = vec[j] - temp;
                vec[j] += temp;
            }
        }
        if (size == n)  // Prevent overflow in 'size *= 2'
            break;
    }
}


void ZFFt::transformBluestein(vector<complex<float> > &vec) {
    // Find a power-of-2 convolution length m such that m >= n * 2 + 1
    size_t n = vec.size();
    size_t m = 1;
    while (m / 2 <= n) {
        if (m > SIZE_MAX / 2)
            throw std::length_error("Vector too large");
        m *= 2;
    }

    // Trignometric table
    vector<complex<float> > expTable(n);
    for (size_t i = 0; i < n; i++) {
        unsigned long long temp = static_cast<unsigned long long>(i) * i;
        temp %= static_cast<unsigned long long>(n) * 2;
        float angle = M_PI * temp / n;
        expTable[i] = std::polar(1.0, (double) -angle);
    }

    // Temporary vectors and preprocessing
    vector<complex<float> > av(m);
    for (size_t i = 0; i < n; i++)
        av[i] = vec[i] * expTable[i];
    vector<complex<float> > bv(m);
    bv[0] = expTable[0];
    for (size_t i = 1; i < n; i++)
        bv[i] = bv[m - i] = std::conj(expTable[i]);

    // Convolution
    vector<complex<float> > cv(m);
    convolve(av, bv, cv);

    // Postprocessing
    for (size_t i = 0; i < n; i++)
        vec[i] = cv[i] * expTable[i];
}


void ZFFt::convolve(
        const vector<complex<float> > &xvec,
        const vector<complex<float> > &yvec,
        vector<complex<float> > &outvec) {

    size_t n = xvec.size();
    if (n != yvec.size() || n != outvec.size())
        throw std::domain_error("Mismatched lengths");
    vector<complex<float> > xv = xvec;
    vector<complex<float> > yv = yvec;
    transform(xv);
    transform(yv);
    for (size_t i = 0; i < n; i++)
        xv[i] *= yv[i];
    inverseTransform(xv);
    for (size_t i = 0; i < n; i++)  // Scaling (because this FFT implementation omits it)
        outvec[i] = xv[i] / static_cast<float>(n);
}


static size_t reverseBits(size_t x, int n) {
    size_t result = 0;
    for (int i = 0; i < n; i++, x >>= 1)
        result = (result << 1) | (x & 1U);
    return result;
}