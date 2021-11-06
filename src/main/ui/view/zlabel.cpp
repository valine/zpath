#include <utility>

#include <utils/zfontstore.h>
#include <utils/zsettingsstore.h>
#include "ui/zlabel.h"

ZLabel::ZLabel(float maxWidth, float maxHeight, string font, string resourcePath)
        : ZView(maxWidth, maxHeight) {
    mFont = ZFontStore::getInstance().getDefaultResource();
}

ZLabel::ZLabel(string label, ZView *parent)
        : ZView(ZView::fillParent, getLineHeight()) {
    mFont = ZFontStore::getInstance().getDefaultResource();
    setText(std::move(label));
    setTextColor(ZSettings::getInstance().getBaseTextColor());
    parent->addSubView(this);

    setWindowHeight(parent->getWindowHeight());
    setWindowWidth(parent->getWindowWidth());
}


ZLabel::ZLabel(float maxWidth, float maxHeight) : ZView(maxWidth, maxHeight) {

}

void ZLabel::onCreate() {
    ZView::onCreate();
}

void ZLabel::setup(const string &font) {
    mFont = font;
    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ZLabel::setColorMode(ColorMode colorMode) {
    ZView::setColorMode(colorMode);
    drawText();
    mTextInvalid = true;
}

void ZLabel::createFBO() {
    glGenFramebuffers(1, &mFBO);
    glGenTextures(1, &mTexBuffer);
    setBackgroundImage(new ZTexture(mTexBuffer));
    //updateFrameSize();
}

void ZLabel::updateFrameSize() {
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glBindTexture(GL_TEXTURE_2D, mTexBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWidth() * mDP, getHeight() * mDP, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexBuffer, 0);
    mFrameInvalid = false;
}

void ZLabel::setBackgroundColor(ZColor color) {
    ZView::setBackgroundColor(color);
    setTextColor(color.getTextColor());
}

void ZLabel::drawText() {
    if (getTextShader() == nullptr || getTextShader()->mID == -1) {
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glViewport(0, 0, getWidth() * mDP, getHeight() * mDP);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat labelScale = 1.0;

    // Activate corresponding render state
    getTextShader()->use();

    vec4 tColor = mTextColor.get(mColorMode);
    glUniform3f(glGetUniformLocation(getTextShader()->mID, "textColor"), tColor.x, tColor.y, tColor.z);

    // Update scale, useful for zooming a view out
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(getWidth()), 0.0f,static_cast<GLfloat>(getHeight()));

    glm::mat4 scaleMat = glm::scale(glm::translate(projection, vec3(0,getHeight() / mDP,0)), vec3(1,-1,1));
    glUniformMatrix4fv(glGetUniformLocation(getTextShader()->mID, "projection"), 1, GL_FALSE,
                       glm::value_ptr(scaleMat));

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    float centerOffset = mLineTopMargin * mDP;

    GLfloat x = getCornerRadius().x * mDP;
    GLfloat y = (-13 * mDP) + 17 - centerOffset;

    float vHeight = getHeight();
    int lineHeight = getLineHeight();
    mPoints.clear();
    mLineIndices.clear();
    int xMargin = 1;
    int yMargin = -getLineHeight() + (9.5 * mDP);

    // Handle cursor position for empty string
    mPoints.emplace_back(((x/mDP) + xMargin), (-(y/mDP) - yMargin));
    if (mText.empty() || mFont.empty()) {

    } else {
        // Iterate through all characters
        std::string::const_iterator c;
        for (c = mText.begin(); c != mText.end(); c++) {
            Character ch = ZFontStore::getInstance().getCharacter(mFont, *c);
            int w = ch.Size.x;
            int h = ch.Size.y;

            float lineOffset = vHeight - lineHeight;
            GLfloat xpos = x + ch.Bearing.x;
            GLfloat ypos = lineOffset + y - (ch.Size.y - ch.Bearing.y);
            char ac = c[0];
            char newLine = '\n';
            if (ac == newLine) {
                y -= lineHeight * mDP;
                x = getCornerRadius().x * mDP;
                mLineIndices.push_back(mPoints.size());
            } else {

                float xdp = xpos / mDP;
                float ydp = ypos / mDP;
                float wdp = (float) w / mDP;
                float hdp = (float) h / mDP;
                // Update VBO for each character
                GLfloat vertices[6][4] = {
                        {xdp,     ydp + hdp, 0.0, 0.0},
                        {xdp,     ydp,     0.0, 1.0},
                        {xdp + wdp, ydp,     1.0, 1.0},

                        {xdp,     ydp + hdp, 0.0, 0.0},
                        {xdp + wdp, ydp,     1.0, 1.0},
                        {xdp + wdp, ydp + hdp, 1.0, 0.0}
                };

                glCullFace(GL_FRONT);
                // Render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                // Update content of VBO memory
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 6 * 4,
                                vertices); // Be sure to use glBufferSubData and not glBufferData

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                // Render quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
                // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                x += ((((float) ch.Advance) / 64.0)) *
                     labelScale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
            }

            mPoints.emplace_back(((x/mDP) + xMargin), (-(y/mDP) - yMargin));

            //if (mLineIndices.empty()) {
               // mFirstLineWidth = x + w;
           // }
        }

        mLineIndices.push_back(mPoints.size());
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glCullFace(GL_BACK);
    mTextInvalid = false;
}

void ZLabel::computeLineWidth() {
    if (mText.empty() || mFont.empty()) {
        return;
    }
    GLfloat x = getCornerRadius().x;
    GLfloat y = 0;
    GLfloat labelScale = 1.0;

    std::string::const_iterator c;
    int lineHeight = getLineHeight();

    for (c = mText.begin(); c != mText.end(); c++) {
        Character ch = ZFontStore::getInstance().getCharacter(mFont, *c);
        int w = ch.Size.x;

        char ac = c[0];
        char newLine = '\n';
        if (ac == newLine) {
            y -= lineHeight;
            x = getCornerRadius().x;
        } else {
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (((float) ch.Advance) / 64.0) *
                 labelScale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }

        if (mLineIndices.empty()) {
            mFirstLineWidth = std::max(mFirstLineWidth, (x + w) / mDP);
        }
    }
}

pair<int,int> ZLabel::getEndPoint() {
    if (mPoints.empty()) {
        return {0,0};
    }
    return mPoints.at(mPoints.size() - 1);
}

vector<pair<int,int>> ZLabel::getPoints() {
    return mPoints;
}

vector<int> ZLabel::getLineIndices() {
    return mLineIndices;
}

int ZLabel::getLineHeight() {
    return 18;
}

string ZLabel::getText() {
    return mText;
}

void ZLabel::draw() {
    if (!mInit) {
        mInit = true;
        setup(ZFontStore::getInstance().getDefaultResource());
        createFBO();
    }
    if (mFrameInvalid) {
        updateFrameSize();
    }

    if (mTextInvalid) {
        drawText();
    }

    if (needsRender()) {
        ZView::draw();
    }
}

void ZLabel::setTextSize(int textSize) {
    mTextSize = textSize;
    invalidate();
}

void ZLabel::setTextColor(ZColor color) {
    mTextColor = color;
    mTextInvalid = true;
    invalidate();
}

ZColor ZLabel::getTextColor() {
    return mTextColor;
}

void ZLabel::setText(string text) {
    mText = std::move(text);
    mTextInvalid = true;
    computeLineWidth();
    invalidate();
}

void ZLabel::setFont(string fontPath) {
    mFontPath = std::move(fontPath);
}

void ZLabel::onSizeChange() {
    ZView::onSizeChange();
    mFrameInvalid = true;
    mTextInvalid = true;
}

float ZLabel::getTextWidth() {
    return mFirstLineWidth;
}
