//
// Created by lukas on 7/14/19.
//

#ifndef ZPATH_ZFONTSTORE_H
#define ZPATH_ZFONTSTORE_H

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
using namespace std;

#include <map>

class ZFontStore {

public:
    static ZFontStore& getInstance(){
        static ZFontStore instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    FT_Face loadFont(string resoucePath);

private:
    ZFontStore();
    static ZFontStore *mInstance;
    FT_Library mFreetype;
    map<string, FT_Face> mFonts;
    ZFontStore(ZFontStore const&);              // Don't Implement
    void operator=(ZFontStore const&); // Don't implement
};


#endif //ZPATH_ZFONTSTORE_H
