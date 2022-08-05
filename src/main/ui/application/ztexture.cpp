#include <utils/zutil.h>
#include "ui/ztexture.h"

#include "utils/zfontstore.h"

ZTexture::ZTexture(string path) {
    mTextureID = loadTexture(path.c_str());
    mTextureIDDark = mTextureID;
}

ZTexture::ZTexture(int id) {
    mTextureID = id;
    mTextureIDDark = id;
}


ZTexture::ZTexture(int id, int idDark) {
    mTextureID = id;
    mTextureIDDark = idDark;
}


ZTexture::ZTexture(float* pixels, int width, int height) {
    mTextureID = loadTexture(pixels, width, height);
    mTextureIDDark = mTextureID;
    mWidth = width;
    mHeight = height;
}

uint ZTexture::getID() {
    return mTextureID;
}

uint ZTexture::getIDDark() {
    return mTextureIDDark;
}

void ZTexture::setID(int id) {
    mTextureID = id;
    mTextureIDDark = id;
}

unsigned int ZTexture::loadTexture(char const * path){
    unsigned int textureID;
    glGenTextures(1, &textureID);

    ZUtil::Image img = ZUtil::loadTexture(path);
    if (img.pixels) {
        GLenum format;
        if (img.compCount == 1)
            format = GL_RED;
        else if (img.compCount == 3)
            format = GL_RGB;
        else if (img.compCount == 4)
            format = GL_RGBA;

        //cout << "width: " << width << " height: " << height << endl;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, img.width, img.height, 0, format, GL_FLOAT, img.pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        ZUtil::freeStbi(img.pixels);
    }

    mWidth = img.width;
    mHeight = img.height;

    return textureID;
}

unsigned int ZTexture::loadTexture(float* pixels, int width, int height){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    GLenum format;
    format = GL_RGB;
    //cout << "width: " << width << " height: " << height << endl;
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, format, GL_FLOAT, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mWidth = width;
    mHeight = height;
    delete[] pixels;
    return textureID;
}

void ZTexture::save() {
    string path = ZSettings::getInstance().getResourcePath() + "/../../../debug.png";
    ZUtil::saveGlTex(path.c_str(), mTextureID, mWidth, mHeight);
}

void ZTexture::setID(int light, int dark) {
    mTextureID = light;
    mTextureIDDark = dark;
}
