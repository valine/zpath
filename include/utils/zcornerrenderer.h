//
// Created by Lukas Valine on 4/1/21.
//

#ifndef ZPATH_ZCORNERRENDERER_H
#define ZPATH_ZCORNERRENDERER_H

#include "ui/zshader.h"
#include "ui/ztexture.h"
#include <vector>

class ZCornerRenderer {


public:

    ZTexture* createTexture(int viewWidth, int viewHeight, vec4 radius) {
        unsigned int texBuffer;
        glGenTextures(1, &texBuffer);
        draw(viewWidth, viewHeight, radius, texBuffer);

        auto* tex = new ZTexture(texBuffer);
        return tex;
    }
    /**
     * Returns texture ID
     * @return Texture ID
     */
    unsigned int draw(int width, int height, vec4 radius, unsigned int texID) {


        glEnable(GL_MULTISAMPLE);

        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);

        glBindVertexArray(mVAO);
        mShader->use();
        mat4 matrix = glm::ortho(-1.0, 1.0, 1.0, -1.0, 1.0, -1.0);
        mShader->setMat4("uMatrix", matrix);
        mShader->setVec4("uRadius", radius);

        mShader->setFloat("uWidth", width);
        mShader->setFloat("uHeight", height);

        glViewport(0,0, width, height);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        int triangles = 2;
        glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, nullptr);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindVertexArray(0);
        return texID;
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
