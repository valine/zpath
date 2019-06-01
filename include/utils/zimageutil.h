
#ifndef ZPATH_ZIMAGEUTIL_H
#define ZPATH_ZIMAGEUTIL_H

#include <string>

using namespace std;

class ZImageUtil {

public:
    static void saveImage(const char *file, float* pixels, int w, int h);
    static void saveGlTex(const char *file, unsigned int tex, int w, int h);
    static void saveGlFBO(const char *file, unsigned int fbo, int w, int h);
};


#endif //ZPATH_ZIMAGEUTIL_H
