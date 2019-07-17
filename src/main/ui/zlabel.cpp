#include <utils/zfontstore.h>
#include "ui/zlabel.h"

ZLabel::ZLabel(float maxWidth, float maxHeight, string font, string resourcePath) 
: ZView(maxWidth, maxHeight) {

        string resourceString = resourcePath + "resources/fonts/" + font ;
        FT_Face face = ZFontStore::getInstance().loadFont(resourceString);

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

string ZLabel::getText() {
    return mText;
}

void ZLabel::draw() {
	ZView::draw();

	int yv = getWindowHeight() - getBottom();
	glViewport(getLeft(),yv,getWidth(),getHeight());
    GLfloat scale = 1.0;
   
	// Activate corresponding render state	
    getTextShader()->use();
    glUniform3f(glGetUniformLocation(getTextShader()->mID, "textColor"), mTextColor.x, mTextColor.y, mTextColor.z);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(getWidth()), 0.0f, static_cast<GLfloat>(getHeight()));
    glUniformMatrix4fv(glGetUniformLocation(getTextShader()->mID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
  
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    GLfloat x = 0;
    GLfloat y = 5 * scale;

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = mText.begin(); c != mText.end(); c++) 
    {
        Character ch = ZFontStore::getInstance().getCharacter(mFont,*c);

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },            
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }           
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 6 * 4, vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ZLabel::setTextSize(int textSize) {
	mTextSize = textSize;
}

void ZLabel::setTextColor(vec3 color) {
	mTextColor = color;
}

void ZLabel::setText(string text) {
	mText = text;
}

void ZLabel::setFont(string fontPath) {
	mFontPath = fontPath;
}