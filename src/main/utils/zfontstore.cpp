#include <utility>

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

FT_Face ZFontStore::loadFont() {
    return loadFont(mDefaultResource, 0, 0);
}

string ZFontStore::getFontKey(string resourcePath, int size) {
    return resourcePath + to_string(size);
}

FT_Face ZFontStore::loadFont(string resourcePath, float dp, int size) {
    if (mFonts.find(getFontKey(resourcePath, size)) == mFonts.end()) {

        mDp = dp;
        FT_Face face;

        string resourceString = resourcePath;
        const char *resourceChar = resourceString.c_str();

        FT_New_Face(mFreetype, resourceChar, 0, &face);


        // Set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, size * dp);
        mFonts.insert(make_pair(getFontKey(resourcePath, size), face));

        // Disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Load first 128 characters of ASCII set
        for (GLubyte c = 0; c < 128; c++) {
            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // Generate texture
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
            );
            // Set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            // Now store character for later use


            Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    (GLuint) face->glyph->advance.x
            };
            string key = to_string(c) + resourcePath + to_string(size);
            mCharacters.insert(make_pair(key, character));

            glBindTexture(GL_TEXTURE_2D, 0);
            // Destroy FreeType once we're finished
        }


        return face;
    }

    return mFonts.at(getFontKey(resourcePath, size));
}

Character ZFontStore::getCharacter(const string &resourcePath, GLchar c, int size) {
    string key = to_string(c) + resourcePath + to_string(size);
    if (mSizesLoaded.count(key) == 0) {
        loadFont(resourcePath, mDp, size);
        mSizesLoaded.insert(make_pair(key, 1));
    }

    if (mCharacters.count(key) == 0) {
        string defaultKey = to_string(' ') + resourcePath + to_string(size);
        return mCharacters.at(defaultKey);
    }
    return mCharacters.at(key);
}

void ZFontStore::setDefaultResource(string r) {
    mDefaultResource = std::move(r);
}

string ZFontStore::getDefaultResource() {
    return mDefaultResource;
}

