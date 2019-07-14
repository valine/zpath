//
// Created by lukas on 7/14/19.
//

#include <iostream>
#include "utils/zfontstore.h"
using namespace std;

ZFontStore::ZFontStore() {
    if (FT_Init_FreeType(&mFreetype)) {
        cout << "ERROR::FREETYPE: Could not init FreeType Library" << endl;
    }
}

FT_Face ZFontStore::loadFont(string resourcePath) {
    if (mFonts.find(resourcePath) == mFonts.end()) {

        FT_Face face;

        string resourceString = resourcePath;
        const char *resourceChar = resourceString.c_str();

        FT_New_Face(mFreetype, resourceChar, 0, &face);
        mFonts.insert(make_pair(resourcePath, face));
        return face;
    }
    return mFonts.at(resourcePath);
}
