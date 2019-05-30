
#ifndef ZPATH_ZIMAGEUTIL_H
#define ZPATH_ZIMAGEUTIL_H

#include <string>

using namespace std;

class ZImageUtil {

public:
    static void saveImage(string file, float* pixels, int w, int h);
    static void saveGlImage(const char *file, unsigned int texPointer, int w, int h);
};


#endif //ZPATH_ZIMAGEUTIL_H
