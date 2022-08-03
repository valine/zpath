//
// Created by lukas on 2/18/21.
//

#ifndef ZPATH_ZGRIDRENDERER_H
#define ZPATH_ZGRIDRENDERER_H


#include <ui/ztexture.h>
#include <vector>
#include <ui/zshader.h>

class ZGridRenderer {

public:
    static ZGridRenderer& get(){
        static ZGridRenderer instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    int draw(int texId, vec4 color) {

        int innerTiles = 16;
        int res = 32;
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res, res, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);

        glBindVertexArray(mVAO);
        mShader->use();
        mShader->setVec4("uColor", color);

        mat4 matrix = glm::ortho(0.0, 1.0, 1.0, 0.0, 1.0, -1.0);
        mShader->setMat4("uMatrix", matrix);

        glViewport(0,0, res, res);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);


        int triangles = 2;
        float tileSize = (float) res / 2.0f;
        float tileSizeHalf = (float) res / (float) innerTiles;

        mShader->setVec4("uColor", color * vec4(1,1,1,0.5));

        glViewport(0,0, tileSize, tileSize);
        glDrawElements(GL_POINTS, triangles * 3, GL_UNSIGNED_INT, nullptr);
        glViewport(tileSize,tileSize,tileSize,tileSize);
      //  glDrawElements(GL_POINTS, triangles * 3, GL_UNSIGNED_INT, nullptr);

        mShader->setVec4("uColor", color * vec4(1,1,1,0.5));

        for (int y = 0; y < innerTiles; y+=1) {
            for (int x = 0; x < innerTiles; x+=2) {

                if (y % 2 == 1) {
                    glViewport((x + 1) * tileSizeHalf,y * tileSizeHalf, tileSizeHalf, tileSizeHalf);
                    glDrawElements(GL_POINTS, triangles * 3, GL_UNSIGNED_INT, nullptr);
                } else {
                    glViewport(x * tileSizeHalf,y * tileSizeHalf, tileSizeHalf, tileSizeHalf);
                   // glDrawElements(GL_POINTS, triangles * 3, GL_UNSIGNED_INT, nullptr);
                }

            }
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return texId;
    }

    void update(ZTexture texture) {
        draw(texture.getID(), vec4(0.1,0.1,0.1,0.5));
        draw(texture.getIDDark(), vec4(vec3(1.0),0.5));
    }

    ZTexture *create() {
        unsigned int buffer;
        glGenTextures(1, &buffer);
        draw(buffer, vec4(0.1,0.1,0.1,0.4));

        unsigned int bufferDark;
        glGenTextures(1, &bufferDark);
        draw(bufferDark, vec4(vec3(1.0),0.6));

        auto* tex = new ZTexture(buffer, bufferDark);
        tex->setFillMode(ZTexture::clip);
        tex->mWidth = 4;
        tex->mHeight = 4;
        tex->setScale(0.1);
        return tex;
    }

private:

    ZGridRenderer() {
        init();
    }

    void init() {
        // Mesh buffers
        vector<float> verts = {
                               1, 0, 1, 0};
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

        // Shader
        mShader = new ZShader(vertex_shader, fragment_shader);
    }

    ZShader* mShader;

    unsigned int mFBO;

    unsigned int mVAO;
    unsigned int mVertBuffer;
    unsigned int mEdgeBuffer;

    // Shader code
    const string vertex_shader =
#include "shaders/grid.vs"
    ;

    const string fragment_shader =
#include "shaders/grid.fs"
    ;


};


#endif //ZPATH_ZGRIDRENDERER_H
