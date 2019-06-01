
#ifndef ZPATH_ZIMAGEUTIL_H
#define ZPATH_ZIMAGEUTIL_H

#include <string>
#include "ui/z3dview.h"

using namespace std;

class ZUtil {

public:
    static void saveImage(const char *file, float* pixels, int w, int h);
    static void saveGlTex(const char *file, unsigned int tex, int w, int h);
    static void saveGlFBO(const char *file, unsigned int fbo, int w, int h);
    static void saveView(Z3DView* v);

    static void chart(float *p, int s);
};


#endif //ZPATH_ZIMAGEUTIL_H
