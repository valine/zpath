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
    virtual void setTextSize(int textSize);
    ZColor getTextColor();
    virtual void setText(string text);
    void setFont(string fontPath);
    string getText();
    void setBackgroundColor(ZColor color) override;
    int getTextSize();

    void drawText();
    pair<int,int> getEndPoint();
    void onSizeChange() override;

    vector<pair<int, int>> getPoints();
    vector<int> getLineIndices();
    void setVisibility(bool visible) override;

    int getLineHeight();

    virtual float getTextWidth();

    void computeLineWidth();
    virtual void setTextColor(ZColor color);

    void setLineTopMargin(float margin) {
        mLineTopMargin = margin;
        mTextInvalid = true;
    }
private:

    bool mInit = false;
    unsigned int mFBO = -1;
    unsigned int mTexBuffer = -1;

    GLuint VAO, VBO;
    int mTextSize = 14;
    float mLineTopMargin = 0;
    ZColor mTextColor = white;
    string mText = "";
    string mFontPath = "";
    string mFont;

    bool mTextInvalid = true;
    bool mFrameInvalid = true;
    float mFirstLineWidth = 0;

    pair<int, int> mEndPoint;

    /**
     * Character coordinates for setting cursor location
     */
    vector<pair<int, int>> mPoints;
    vector<int> mLineIndices;


    void createFBO();

    void setup(const string &font);
    void updateFrameSize();


    void onCreate();

    void setColorMode(ColorMode colorMode);

};

#endif
