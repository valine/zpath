//
// Created by lukas on 2/18/21.
//

#ifndef ZPATH_ZSHADOWRENDERER_H
#define ZPATH_ZSHADOWRENDERER_H


#include <ui/ztexture.h>
#include <vector>
#include <ui/zshader.h>

class ZShadowRenderer {

public:
    static ZShadowRenderer& get(){
        static ZShadowRenderer instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    ZTexture* createShadow(int viewWidth, int viewHeight, float radius) {

        int shadowWidth = viewWidth + (int) radius;
        int shadowHeight = viewHeight + (int) radius;
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
        glBindTexture(GL_TEXTURE_2D, mTexBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, shadowWidth, shadowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexBuffer, 0);

        glBindVertexArray(mVAO);

        mShader->use();

        double margin = 0.1;
        mat4 matrix = glm::ortho(0.0 - margin, 1.0 + margin, 1.0 + margin, 0.0 - margin, 1.0, -1.0);
        mShader->setMat4("uMatrix", matrix);

        glViewport(0,0, shadowWidth, shadowHeight);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        int triangles = 2;
        glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        auto* tex = new ZTexture(mTexBuffer);
        return tex;
    }

private:

    ZShadowRenderer() {
        init();
    }

    ZShader* mShader;

    unsigned int mFBO;
    unsigned int mTexBuffer;

    unsigned int mVAO;
    unsigned int mVertBuffer;
    unsigned int mEdgeBuffer;


    // Shader code
    const string shadow_vs =
#include "shaders/shadow.vs"
    ;

    const string shadow_fs =
#include "shaders/shadow.fs"
    ;


    void init() {
        // Mesh buffers
        vector<float> verts = {0, 0, 0, 0,
                               1, 0, 1, 0,
                               0, 1, 0, 1,
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
        glGenTextures(1, &mTexBuffer);

        // Shader
        mShader = new ZShader(shadow_vs, shadow_fs);
    }



};


#endif //ZPATH_ZSHADOWRENDERER_H
