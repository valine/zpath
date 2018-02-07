#include "zview.h"

#include <iostream>
#include <map>
#include <string>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

class ZLabel : public ZView {
	
	public:

		ZLabel(float maxWidth, float maxHeight, string font, string resourcePath);
		void draw();

		void setTextSize(int textSize);
		void setTextColor(vec3 color);
		void setText(string text);
		void setFont(string fontPath);
	private: 
		GLuint VAO, VBO;
		map<GLchar, Character> Characters;

		int mTextSize = 14;
		vec3 mTextColor = vec3(1,1,1);
		string mText = "";
		string mFontPath = "";
};
