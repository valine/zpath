#include "mesh/zrenderer.h"


ZRenderer::ZRenderer(string resourcePath) {
    mShader = new ZShader(base_vs, base_fs);
    mBackgroundShader = new ZShader(background_vs, background_fs);
    mColorTextureShader = new ZShader(base_vs, color_texture_fs);
    mSelectionShader = new ZShader(selection_vs, selection_fs);
    mHDRShader = new ZShader(fbo_vs, fbo_fs);

    glDepthFunc(GL_LEQUAL);
    mCamera = new ZCamera();
}

void ZRenderer::init() {
    mShader->use();
    mShader->setInt("irradianceMap", 0);
    mShader->setInt("prefilterMap", 1);
    mShader->setInt("brdfLUT", 2);

    glGenFramebuffers(1, &mMainFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mMainFBO);
    //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  
    glGenTextures(1, &mMainBuffer);
    glBindTexture(GL_TEXTURE_2D, mMainBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mCamera->getWidth(), mCamera->getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenRenderbuffers(1, &mRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mCamera->getWidth(), mCamera->getHeight());

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mMainBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Main Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create selection offscreen buffer
    glGenFramebuffers(1, &mSelectionFBO);
    glGenTextures(1, &mSelectionBuffer);
    glBindTexture(GL_TEXTURE_2D, mSelectionBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mCamera->getWidth(), mCamera->getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenRenderbuffers(1, &mSelectionRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mSelectionRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mCamera->getWidth(), mCamera->getHeight());

    glBindFramebuffer(GL_FRAMEBUFFER, mSelectionFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSelectionBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mSelectionRenderBuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Selection Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (mRenderToTexture) {
        // Final texture
        glGenFramebuffers(1, &mFinalFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, mFinalFBO);
        glGenTextures(1, &mFinalBuffer);
        glBindTexture(GL_TEXTURE_2D, mFinalBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mCamera->getWidth(), mCamera->getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFinalBuffer, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBuffer);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Final Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    mHDRShader->use();
    mHDRShader->setInt("hdrBuffer", 0);

    // Transparency 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBlendEquation (GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void ZRenderer::setRenderToTexture(bool toTexture) {
    mRenderToTexture = toTexture;
}

// Experimental features
void ZRenderer::onDrawFinshed() {
//renderQuad();
//renderCube();
}

void ZRenderer::draw() {
    if (mScene != nullptr) {
        if (mParentView->getVisibility()) {

    
            renderMain();
            renderSelection();
            renderToScreen();

            glDisable(GL_DEPTH_TEST);
        }
    }
}

void ZRenderer::recreateBuffers() {
    float width = mCamera->getWidth();
    float height =  mCamera->getHeight();

    glBindTexture(GL_TEXTURE_2D, mMainBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, mSelectionBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glBindRenderbuffer(GL_RENDERBUFFER, mSelectionRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

mat4 ZRenderer::getModelMatrix(ZObject* object) {

    mat4 modelMatrix = mat4();
    modelMatrix = scale(modelMatrix, object->getScale());
    modelMatrix = translate(modelMatrix, object->getTranslation());

    mat4 billboard = mat4(1);

    if (object->isBillboard()) {
        vec4 objectCenterTmp = modelMatrix * vec4(object->getOrigin().x, object->getOrigin().y, object->getOrigin().z, 1.0);
        vec3 objectCenter = vec3(objectCenterTmp.x, objectCenterTmp.y, objectCenterTmp.z);

        mat4 snapToCenter = mat4(1);
        snapToCenter = inverse(translate(snapToCenter, objectCenter));

        mat4 lookAt = glm::lookAt(
            vec3(0), // Camera is at (4,3,3), in World Space
            mCamera->getBillboardTarget() - objectCenter, // and looks at the origin
            mCamera->getUp()  // Head is up (set to 0,-1,0 to look upside-down)
        );

        mat4 retranslation = mat4(1);
        retranslation = translate(retranslation, objectCenter);

       
        billboard = retranslation * inverse(lookAt) * snapToCenter;
        return billboard * modelMatrix;
    } 

    return modelMatrix;
}

unsigned int ZRenderer::getMainTexture() {
    return mFinalBuffer;
}


void ZRenderer::renderMain() {
    float width = mCamera->getWidth();
    float height =  mCamera->getHeight();

    // Render to 16 bit frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, mMainFBO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0,0, width, height);
    glEnable(GL_DEPTH_TEST);

    // Draw background
    mBackgroundShader->use();
    mBackgroundShader->setMat4("projection", mCamera->getProjectionMatrix());
    mBackgroundShader->setMat4("view", mCamera->getViewMatrix());
    mBackgroundShader->setVec3("uColorFactor", mScene->getWorld()->getColor());
    glActiveTexture(GL_TEXTURE0);
    if (mScene->getWorld()->isBackgroundBlurred()) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, mScene->getWorld()->getIrradienceID());
    } else {
        glBindTexture(GL_TEXTURE_CUBE_MAP, mScene->getWorld()->getBackgroundID());
    }

    renderCube();

    vector<ZPointLight*> lights = mScene->getLights();
    vector<ZObject*> objects = mScene->getObjects();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mScene->getWorld()->getIrradienceID());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mScene->getWorld()->getPrefilteredID());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mScene->getWorld()->getBrdfLutID());

    ZShader* shader;
    shader = mShader;
    shader->use();
    shader->setVec3("uWorldColor", mScene->getWorld()->getColor());
    shader->setVec3("uLightPositions", (uint) lights.size(), mScene->getLightPositions());
    shader->setVec3("uLightColors", (uint) lights.size(), mScene->getLightColors());

    shader->setMat4("uProjectionMatrix", mCamera->getProjectionMatrix());
    shader->setMat4("uViewMatrix", mCamera->getViewMatrix());

    int mPositionLocation = glGetAttribLocation(shader->mID, "aPos");
    int mNormalLocation = glGetAttribLocation(shader->mID, "aNormal");
    int mTextureCoordLocation = glGetAttribLocation(shader->mID, "aTextureCoords");

    mat4 identityMatrix = mat4();
    shader->setMat4("uModelMatrix", identityMatrix);

    int objectIndex = 0;
    for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
        ZObject *object = (*it);
        ZMesh *mesh = (*it)->getMesh();
        ZMaterial* material = object->getMaterial();

        shader->setMat4("uModelMatrix", getModelMatrix(object));

        if (material->getColorTexture() != nullptr) {
            shader = mColorTextureShader;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material->getColorTexture()->getID());
            shader->use();
        } 

        glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexBuffer());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getFaceIndicesBuffer());

        glEnableVertexAttribArray(mPositionLocation);
        glVertexAttribPointer(mPositionLocation, 3, GL_FLOAT, GL_FALSE,
          sizeof(float) * 3, (void*) 0);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexNormalBuffer());

        glEnableVertexAttribArray(mNormalLocation);
        glVertexAttribPointer(mNormalLocation, 3, GL_FLOAT, GL_FALSE,
          sizeof(float) * 3, (void*) 0);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->getTextureCoordinatesBuffer());
        glEnableVertexAttribArray(mTextureCoordLocation);
        glVertexAttribPointer(mTextureCoordLocation, 2, GL_FLOAT, GL_FALSE,
          sizeof(float) * 2, (void*) 0);

        vec4 color = material->getColor();
        shader->setVec4("uColor", color.r, color.g, color.b, color.a);

        float selected = 0;
        if (mScene->getActiveObjectIndex() == objectIndex) {
            selected = 1;
        }

        shader->setFloat("uMetallic", material->getMetallic());
        shader->setFloat("uSelected", selected);
        shader->setFloat("uRoughness", material->getRoughness());

        shader->setVec3("uCameraPosition", mCamera->getPosition());

        glDrawElements(GL_TRIANGLES, mesh->getFaceIndiceCount(), GL_UNSIGNED_INT, nullptr); 
        objectIndex++;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    onDrawFinshed();
}

void ZRenderer::renderSelection() {
    if (!mRenderToTexture) {
        glBindFramebuffer(GL_FRAMEBUFFER, mSelectionFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mSelectionShader->use();
        mSelectionShader->setMat4("uProjectionMatrix", mCamera->getProjectionMatrix());
        mSelectionShader->setMat4("uViewMatrix", mCamera->getViewMatrix());

        vector<ZObject*> objects = mScene->getObjects();

        int mPositionLocation = glGetAttribLocation(mSelectionShader->mID, "aPos");

        int objectIndex = 0;
        for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
            ZMesh *mesh = (*it)->getMesh();

            mSelectionShader->setMat4("uModelMatrix", getModelMatrix((*it)));

            glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexBuffer());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getFaceIndicesBuffer());

            glEnableVertexAttribArray(mPositionLocation);
            glVertexAttribPointer(mPositionLocation, 3, GL_FLOAT, GL_FALSE,
              sizeof(float) * 3, (void*) 0);

            vec4 color = vec4((float) objectIndex / 256,0,0,1.0);
            mSelectionShader->setVec4("uColor", color.r, color.g, color.b, color.a);

            glDrawElements(GL_TRIANGLES, mesh->getFaceIndiceCount(), GL_UNSIGNED_INT, nullptr); 

            objectIndex++;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void ZRenderer::renderToScreen() {

    if (mRenderToTexture) {
        glBindFramebuffer(GL_FRAMEBUFFER, mFinalFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mHDRShader->use();
        glViewport(0,0,mCamera->getWidth(),mCamera->getHeight());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mMainBuffer);

        mHDRShader->setBool("hdr", true);
        mHDRShader->setFloat("exposure", mScene->getExposure());

        renderQuad();
    } else {

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        mHDRShader->use();
        int yv = mParentView->getWindowHeight() - mParentView->getBottom();
        glViewport(mParentView->getLeft(),yv,mParentView->getWidth(),mParentView->getHeight());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mMainBuffer);
        mHDRShader->setBool("hdr", true);
        mHDRShader->setFloat("exposure", mScene->getExposure());
        renderQuad();
    }
}

int ZRenderer::getObjectIndexAtLocation(int x, int y) {
    GLubyte rgba[4];
    glBindFramebuffer(GL_FRAMEBUFFER, mSelectionFBO);
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    if (rgba[3] == 0) {
        return -1;
    }
    return (int) (rgba[0]);
}

ZScene* ZRenderer::getScene() {
    return mScene;
}

void ZRenderer::setScene(ZScene *scene) {
    mScene = scene;
}

void ZRenderer::setParentView(ZView* view) {
    mParentView = view;
}

ZCamera* ZRenderer::getCamera() {
    return mCamera;
}

void ZRenderer::renderQuad() {
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void ZRenderer::renderCube() {
    // initialize (if necessary)
    if (cubeVAO == 0) {
        float vertices[] = {
        // back face
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         

        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left

        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right

        1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left

        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left


        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right

        // right face
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
        1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right     
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right


        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
        1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left

        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left


        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right


        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
        1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right


        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,// bottom-left      
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left

        };

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        }
        // render Cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
}
