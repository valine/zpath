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

    void draw() override;
    void setTextSize(int textSize);
    void setTextColor(vec3 color);
    void setText(string text);
    void setFont(string fontPath);
    string getText();

    pair<int,int> getEndPoint();
    void onSizeChange() override;
private:

    unsigned int mFBO = -1;
    unsigned int mTexBuffer = -1;

    GLuint VAO, VBO;
    int mTextSize = 14;
    vec3 mTextColor = vec3(1,1,1);
    string mText = "";
    string mFontPath = "";
    string mFont;

    bool mTextInvalid = true;
    bool mFrameInvalid = true;

    /**
     * Character coordinates for setting cursor location
     */
    vector<pair<int, int>> mPoints;


    void createFBO();
    void drawText();
    void setup(const string &font, const string &resourcePath);
    void updateFrameSize();
};

#endif
