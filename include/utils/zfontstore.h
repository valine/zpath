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
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

class ZFontStore {

public:
    static ZFontStore& getInstance(){
        static ZFontStore instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    FT_Face loadFont(string resoucePath);
    Character getCharacter(string resourcePath, GLchar);

    string getDefaultResource();
    void setDefaultResource(string);
    FT_Face loadFont();
private:
    ZFontStore();
    static ZFontStore *mInstance;
    FT_Library mFreetype;
    map<pair<GLchar, string>, Character> mCharacters;
    map<string, FT_Face> mFonts;
    ZFontStore(ZFontStore const&);              // Don't Implement
    void operator=(ZFontStore const&); // Don't implement
    std::string mDefaultResource = "";
};


#endif //ZPATH_ZFONTSTORE_H
