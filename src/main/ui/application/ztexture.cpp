#include <utils/zutil.h>
#include "ui/ztexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "ui/stb_image.h"
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

    int width, height, nrComponents;
    float *data = stbi_loadf(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        //cout << "width: " << width << " height: " << height << endl;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, format, GL_FLOAT, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        //std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    mWidth = width;
    mHeight = height;

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
