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


        // Set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 14);

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
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Now store character for later use
            Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    (GLuint) face->glyph->advance.x
            };
            make_pair(c, resourcePath);
            mCharacters.insert(pair<pair<GLchar, string>, Character>(make_pair(c, resourcePath), character));

            glBindTexture(GL_TEXTURE_2D, 0);
            // Destroy FreeType once we're finished
        }


        return face;
    }

    return mFonts.at(resourcePath);
}

Character ZFontStore::getCharacter(string resourcePath, GLchar c) {
    return mCharacters.at(pair<GLchar, string>(c, resourcePath));
}

