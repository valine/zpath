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

    void updateShadow(ZTexture texture, int viewWidth, int viewHeight, float radius) {

        
    }

    ZTexture* createShadow(int viewWidth, int viewHeight, float radius) {

        int shadowWidth = viewWidth + (int) radius;
        int shadowHeight = viewHeight + (int) radius;
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
        glBindTexture(GL_TEXTURE_2D, mTexBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, shadowWidth, shadowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexBuffer, 0);

        glBindVertexArray(mVAO);
        mShader->use();


        double marginSide = radius / viewWidth;
        double marginTop = radius / viewHeight;
        mat4 matrix = glm::ortho(0.0 - marginSide, 1.0 + marginSide, 1.0 + marginTop, 0.0 - marginTop, 1.0, -1.0);
        mShader->setMat4("uMatrix", matrix);

        glViewport(0,0, shadowWidth, shadowHeight);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        int triangles = 2;
        glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, nullptr);


        // Render blur
        mBlurShader->use();
        mat4 blurMat = glm::ortho(0.0, 1.0, 1.0, 0.0, 1.0, -1.0);
        mBlurShader->setMat4("uMatrix", blurMat);
        mBlurShader->setFloat("uRadiusX", marginSide);
        mBlurShader->setFloat("uRadiusY", marginTop);

        glBindTexture(GL_TEXTURE_2D, mBlurBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, shadowWidth, shadowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBlurBuffer, 0);

        glViewport(0,0, shadowWidth, shadowHeight);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTexBuffer);

        glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        auto* tex = new ZTexture(mBlurBuffer);
        return tex;
    }

private:

    ZShadowRenderer() {
        init();
    }

    ZShader* mShader;
    ZShader* mBlurShader;

    unsigned int mFBO;
    unsigned int mTexBuffer;
    unsigned int mBlurBuffer;

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

    const string shadow_blur_fs =
#include "shaders/shadowblur.fs"
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

        glGenTextures(1, &mBlurBuffer);

        // Shader
        mShader = new ZShader(shadow_vs, shadow_fs);
        mBlurShader = new ZShader(shadow_vs, shadow_blur_fs);
    }



};


#endif //ZPATH_ZSHADOWRENDERER_H
