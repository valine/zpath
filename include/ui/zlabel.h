#ifndef ZLABEL_H
#define ZLABEL_H

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

class ZLabel : public ZView {
	
	public:

		ZLabel(float maxWidth, float maxHeight, string font, string resourcePath);
        ZLabel(float maxWidth, float maxHeight);

    ZLabel(string label, ZView *parent);

    void draw();

		void setTextSize(int textSize);
		void setTextColor(vec3 color);
		void setText(string text);
		void setFont(string fontPath);
		string getText();
	private: 
		GLuint VAO, VBO;
		int mTextSize = 14;
		vec3 mTextColor = vec3(1,1,1);
		string mText = "";
		string mFontPath = "";
		bool mInvalid = true;
		string mFont;

    void setup(const string &font, const string &resourcePath);
};

#endif
