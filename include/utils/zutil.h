
#ifndef ZPATH_ZUTIL_H
#define ZPATH_ZUTIL_H

#include <string>
#include "ui/z3dview.h"

using namespace std;

class ZUtil {

public:
    static void saveImage(const char *file, float* pixels, int w, int h);
    static void saveImage(const char *file, double* pixels, int w, int h);
    static void saveImageR(const char *file, float* pixels, int w, int h);
    static void saveImageR(const char *file, double* pixels, int w, int h);
    static void saveGlTex(const char *file, unsigned int tex, int w, int h);
    static void saveGlFBO(const char *file, unsigned int fbo, int w, int h);
    static void saveGlFBOMain(const char *file, int x, int y, int w, int h);
    static void saveView(ZView* v);

    static void chart(const char *file, float *p, int s, float max);
    static void chart(float *points, int size, float max);
    static void chart(const char *file, double *p, int s, float max);
    static void chart(double *points, int size, float max);
    static void chart(double *p, int size, int mod, int offset, float max);

    static string getHomeFolder();

    static string replace(string subject, const string &search, const string &replace);

    static vector<string> split(const string &s, char delim) {
        char escape = '\\';
        vector<string> result;
        stringstream ss (s);
        string item;

        string last = "";
        while (getline(ss, item, delim)) {

            // Delimiter was escaped
            if (!last.empty() && !result.empty() &&
                last.at(last.size() - 1) == escape) {

                last = item;
                int rsize = result.size() - 1;
                result.at(rsize).erase( result.at(rsize).size() - 1);
                result.at(result.size() - 1) += delim + item;
            } else {
                result.push_back(item);
                last = item;
            }

        }
        return result;
    }
};


#endif //ZPATH_ZUTIL_H
