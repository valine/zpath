
#ifndef ZPATH_ZIMAGEUTIL_H
#define ZPATH_ZIMAGEUTIL_H

#include <string>
#include "ui/z3dview.h"

using namespace std;

class ZUtil {

public:
    static void saveImage(const char *file, float* pixels, int w, int h);
    static void saveImage(const char *file, double* pixels, int w, int h);
    static void saveGlTex(const char *file, unsigned int tex, int w, int h);
    static void saveGlFBO(const char *file, unsigned int fbo, int w, int h);
    static void saveGlFBOMain(const char *file, int x, int y, int w, int h);
    static void saveView(ZView* v);

    static void chart(const char *file, float *p, int s);
    static void chart(float *p, int s);
    static void chart(const char *file, double *p, int s);
    static void chart(double *p, int s);
    static void chart(double *p, int s, int mod, int offset);

    static string getHomeFolder();
};


#endif //ZPATH_ZIMAGEUTIL_H
