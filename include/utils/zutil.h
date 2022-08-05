
#ifndef ZPATH_ZUTIL_H
#define ZPATH_ZUTIL_H

#include <string>
#include "ui/z3dview.h"

using namespace std;

class ZUtil {

public:
    struct Image {
        float* pixels;
        int width;
        int height;
        int compCount;
    };

    static Image loadTexture(const char * path);

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


    static string getFileExtension(string path) {
        return path.substr(path.find_last_of(".") + 1) ;
    }

    static bool isNumber(const std::string& str) {
        int i = 0, j = str.length() - 1;

        // Handling whitespaces
        while (i < str.length() && str[i] == ' ') {
            i++;
        }
        while (j >= 0 && str[j] == ' ') {
            j--;
        }
        if (i > j) {
            return false;
        }

        if (i == j && !(str[i] >= '0' && str[i] <= '9')) {
            return false;
        }

        if (str[i] != '.' && str[i] != '+'
            && str[i] != '-' && !(str[i] >= '0' && str[i] <= '9')) {
            return false;
        }

        // To check if a '.' or 'e' is found in given
        // string. We use this flag to make sure that
        // either of them appear only once.
        bool flagDotOrE = false;

        for (i; i <= j; i++) {
            // If any of the char does not belong to
            // {digit, +, -, ., e}
            if (str[i] != 'e' && str[i] != '.'
                && str[i] != '+' && str[i] != '-'
                && !(str[i] >= '0' && str[i] <= '9')) {
                return false;
            }

            if (str[i] == '.') {
                // checks if the char 'e' has already
                // occurred before '.' If yes, return 0.
                if (flagDotOrE) {
                    return false;
                }

                // If '.' is the last character.
                if (i + 1 > str.length()) {
                    return false;
                }

                // if '.' is not followed by a digit.
                if (!(str[i + 1] >= '0' && str[i + 1] <= '9')) {
                    return false;
                }
            }

            else if (str[i] == 'e') {
                // set flagDotOrE = 1 when e is encountered.
                flagDotOrE = true;

                // if there is no digit before 'e'.
                if (!(str[i - 1] >= '0' && str[i - 1] <= '9')) {
                    return false;
                }

                // If 'e' is the last Character
                if (i + 1 > str.length()) {
                    return false;
                }

                // if e is not followed either by
                // '+', '-' or a digit
                if (str[i + 1] != '+' && str[i + 1] != '-'
                    && (str[i + 1] >= '0' && str[i] <= '9')) {
                    return false;
                }
            }
        }

        /* If the string skips all above cases, then
        it is numeric*/
        return true;
    }

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

    static void freeStbi(float *data);
};


#endif //ZPATH_ZUTIL_H
