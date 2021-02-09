#include <utility>

#include <utils/zfontstore.h>
#include <utils/zsettingsstore.h>
#include "ui/zlabel.h"

ZLabel::ZLabel(float maxWidth, float maxHeight, string font, string resourcePath)
        : ZView(maxWidth, maxHeight) {
    setup("resources/fonts/" + font, resourcePath);
    createFBO();
}

ZLabel::ZLabel(string label, ZView *parent)
        : ZView(ZView::fillParent, 21) {
    setOffset(5,0);
    setText(std::move(label));
    setTextColor(ZSettingsStore::getInstance().getBaseTextColor());
    parent->addSubView(this);
    setBackgroundColor(parent->getBackgroundColor());

    setWindowHeight(parent->getWindowHeight());
    setWindowWidth(parent->getWindowWidth());

    mFont = ZFontStore::getInstance().getDefaultResource();
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

    createFBO();
}

void ZLabel::setup(const string &font, const string &resourcePath) {
    string resourceString = resourcePath + font;
    ZFontStore::getInstance().loadFont(resourceString);

    mFont = resourceString;
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

ZLabel::ZLabel(float maxWidth, float maxHeight) : ZView(maxWidth, maxHeight) {
    string fontPath = "";
#if defined(_WIN32) || defined(WIN32)
    fontPath = "C:\\windows\\fonts";
#elif __APPLE__

    fontPath = "/System/Library/Fonts/";
    setup("SFNS.ttf", fontPath);
#else
    fontPath = "/usr/share/fonts/truetype/liberation/";
        setup("LiberationSans-Regular.ttf", fontPath);
#endif

    createFBO();
}


void ZLabel::createFBO() {
    glGenFramebuffers(1, &mFBO);
    glGenTextures(1, &mTexBuffer);
    updateFrameSize();
}

void ZLabel::updateFrameSize() {
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glBindTexture(GL_TEXTURE_2D, mTexBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWidth(), getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexBuffer, 0);
    setBackgroundImage(new ZTexture(mTexBuffer));
}

void ZLabel::drawText() {

    if (getTextShader() == nullptr || getTextShader()->mID == -1) {
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glViewport(0, 0, getWidth(), getHeight());

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat labelScale = 1.0;

    // Activate corresponding render state
    getTextShader()->use();
    glUniform3f(glGetUniformLocation(getTextShader()->mID, "textColor"), mTextColor.x, mTextColor.y, mTextColor.z);

    // Update scale, useful for zooming a view out
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(getWidth()), 0.0f,static_cast<GLfloat>(getHeight()));

    glm::mat4 scaleMat = glm::scale(glm::translate(projection, vec3(0,getHeight(),0)), vec3(1,-1,1));
    glUniformMatrix4fv(glGetUniformLocation(getTextShader()->mID, "projection"), 1, GL_FALSE,
                       glm::value_ptr(scaleMat));

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    GLfloat x = 0;
    GLfloat y = 5 * labelScale;

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = mText.begin(); c != mText.end(); c++) {
        Character ch = ZFontStore::getInstance().getCharacter(mFont, *c);

        GLfloat xpos = x + ch.Bearing.x * labelScale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * labelScale;

        GLfloat w = ch.Size.x * labelScale;
        GLfloat h = ch.Size.y * labelScale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
                {xpos,     ypos + h, 0.0, 0.0},
                {xpos,     ypos,     0.0, 1.0},
                {xpos + w, ypos,     1.0, 1.0},

                {xpos,     ypos + h, 0.0, 0.0},
                {xpos + w, ypos,     1.0, 1.0},
                {xpos + w, ypos + h, 1.0, 0.0}
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
        x += (ch.Advance >> 6) *
             labelScale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glCullFace(GL_BACK);
    mTextInvalid = false;
}

string ZLabel::getText() {
    return mText;
}

void ZLabel::draw() {
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



void ZLabel::setTextColor(vec3 color) {
    mTextColor = color;
    invalidate();
}

void ZLabel::setText(string text) {
    mText = std::move(text);
    mTextInvalid = true;
    invalidate();
}

void ZLabel::setFont(string fontPath) {
    mFontPath = std::move(fontPath);
}

void ZLabel::onSizeChange() {
    ZView::onSizeChange();
    updateFrameSize();
    mTextInvalid = true;
}

