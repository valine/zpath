#include "mesh/ztexture3d.h"

#include "ui/stb_image.h"

ZTexture3D::ZTexture3D(string paths[]) {
    mTextureID = loadTexture(paths);
}

uint ZTexture3D::getID() {
    return mTextureID;
}

unsigned int ZTexture3D::loadTexture(string paths[]){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    size_t depth = sizeof(paths)/sizeof(paths[0]);
    int width, height, nrComponents;
    unsigned char *data = stbi_load(paths[0].c_str(), &width, &height, &nrComponents, 0);
   
    unsigned char *allData = (unsigned char*) malloc(sizeof(unsigned char) * width * height * depth);
    int pos = 0;
    for (uint i = 0; i < depth; i++) {
        int subwidth, subheight, subNrComponents;
        unsigned char *subdata = stbi_load(paths[0].c_str(), &width, &subheight, &nrComponents, 0);
        memcpy(data + pos, subdata, sizeof(subdata));
        pos+=sizeof(subdata);
    }
    
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_3D, textureID);
        glTexImage3D(GL_TEXTURE_3D, 0, format, width, height, depth, 0, format, GL_UNSIGNED_BYTE, allData);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << paths << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
