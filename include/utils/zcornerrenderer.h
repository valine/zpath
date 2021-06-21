//
// Created by Lukas Valine on 4/1/21.
//

#ifndef ZPATH_ZCORNERRENDERER_H
#define ZPATH_ZCORNERRENDERER_H

#include "ui/zshader.h"
#include "ui/ztexture.h"
#include <vector>
#include <unordered_map>

class ZCornerRenderer {


public:

    ZTexture* createTexture(int viewWidth, int viewHeight, vec4 color, vec4 outlineColor, float lineWidth, vec4 radius) {
        unsigned int texBuffer;
        glGenTextures(1, &texBuffer);
        auto* tex = new ZTexture(texBuffer);
        draw(viewWidth, viewHeight, radius, color, outlineColor, lineWidth, tex, true);
        tex->mWidth = viewWidth;
        tex->mHeight = viewHeight;
        return tex;
    }

    /**
     * Returns texture ID
     * @return Texture ID
     */
    unsigned int draw(int width, int height, vec4 radius, vec4 color, vec4 outlineColor,
                      float lineWidth, ZTexture* texture, bool cacheResult) {
        long key = getKey(width, height, radius, color, outlineColor, lineWidth);
        if (mTextureMap.find(key) != mTextureMap.end()) {
            int cached = mTextureMap.at(key);
            texture->setID(cached);
            return cached;
        } else {
            glEnable(GL_MULTISAMPLE);

            if (mKeyMap.find(texture->getID()) != mKeyMap.end()) {
                unsigned int texBuffer;
                glGenTextures(1, &texBuffer);
                texture->setID(texBuffer);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
            glBindTexture(GL_TEXTURE_2D, texture->getID());
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getID(), 0);

            glBindVertexArray(mVAO);
            mShader->use();
            mat4 matrix = glm::ortho(-1.0, 1.0, 1.0, -1.0, 1.0, -1.0);
            mShader->setMat4("uMatrix", matrix);
            mShader->setVec4("uRadius", radius);

            mShader->setVec4("uColor", color);
            mShader->setFloat("uWidth", width);
            mShader->setFloat("uHeight", height);
            mShader->setVec4("uOutlineColor", outlineColor);

            if (outlineColor.a == 0) {
                mShader->setFloat("uLineWidth", 0);
            } else {
                mShader->setFloat("uLineWidth", lineWidth);
            }

            glViewport(0,0, width, height);
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT);

            int triangles = 2;
            glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, nullptr);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);

            if (cacheResult) {
                mTextureMap.insert({key, texture->getID()});
                mKeyMap.insert({texture->getID(), key});
            }
        }

        return 0;
    }

    static ZCornerRenderer& get(){
        static ZCornerRenderer instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

private:


    ZCornerRenderer() {
        init();
    }

    vector<float> mTmpVec = vector<float>(16);
    long getKey(int width, int height, vec4 radius, vec4 color, vec4 outline, float oWidth) {
        mTmpVec.at(0) = width;
        mTmpVec.at(1) = height;
        mTmpVec.at(2) = radius.x;
        mTmpVec.at(3) = radius.y;
        mTmpVec.at(4) = radius.z;
        mTmpVec.at(5) = radius.a;

        mTmpVec.at(6) = color.x;
        mTmpVec.at(7) = color.y;
        mTmpVec.at(8) = color.z;
        mTmpVec.at(9) = color.a;

        mTmpVec.at(10) = outline.x;
        mTmpVec.at(11) = outline.y;
        mTmpVec.at(12) = outline.z;
        mTmpVec.at(13) = outline.a;

        mTmpVec.at(15) = oWidth;

        unsigned long hashValue = 0;

        for (int i = 0; i < mTmpVec.size(); i++) {
            int multiple = 500;
            int intValue = (int) (floor(mTmpVec.at(i) * 1000));
            hashValue += (intValue * (long) pow(multiple, i));
        }
        return hashValue;
    }

    unordered_map<long, unsigned int> mTextureMap;
    unordered_map<unsigned int, long> mKeyMap;

    ZShader* mShader;

    unsigned int mFBO;

    unsigned int mVAO;
    unsigned int mVertBuffer;
    unsigned int mEdgeBuffer;

    // Shader code
    const string vertex =
#include "shaders/shadow.vs"
    ;

    const string fragment =
#include "shaders/squircle.glsl"
    ;

    void init() {
        vector<float> verts = {-1, -1, 0, 0,
                               1, -1, 1, 0,
                               -1, 1, 0, 1,
                               1, 1, 1, 1};
        vector<int> edges = {0,2,1, 1,2,3};

        glGenBuffers(1, &mVertBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mVertBuffer);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);

        glGenBuffers(1, &mEdgeBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEdgeBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(int), &edges[0], GL_DYNAMIC_DRAW);

        // Vertex array object
        glGenVertexArrays(1, &mVAO);
        glBindVertexArray(mVAO);
        unsigned int posLocation = 0;
        int vertexCount = 4;
        glBindBuffer(GL_ARRAY_BUFFER, mVertBuffer);
        glEnableVertexAttribArray(posLocation);
        glVertexAttribPointer(posLocation, 4, GL_FLOAT, GL_FALSE,
                              sizeof(float) * vertexCount, nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEdgeBuffer);
        glBindVertexArray(0);

        // Frame buffer and texture
        glGenFramebuffers(1, &mFBO);
        mShader = new ZShader(vertex, fragment);
    }
};


#endif //ZPATH_ZCORNERRENDERER_H
