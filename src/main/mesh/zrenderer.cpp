#include "mesh/zrenderer.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <utils/zimageutil.h>

ZRenderer::ZRenderer(string resourcePath) {
    mShader = new ZShader(base_vs, base_fs);
    mBackgroundShader = new ZShader(background_vs, background_fs);
    mColorTextureShader = new ZShader(base_vs, color_texture_fs);
    mSelectionShader = new ZShader(selection_vs, selection_fs);
    mHDRShader = new ZShader(fbo_vs, fbo_fs);

    glDepthFunc(GL_LEQUAL);
    mCamera = new ZCamera();
}

// Experiments, like rotating objects to test features 
void ZRenderer::onDrawFinshed() {
//renderQuad();
//renderCube();

    // if ( mScene->getObjects().size() > 0) { 
    //     ZObject* object = mScene->getObjects().at(0);

    //     object->setRotation(vec3(0,0,1));
    //     mScene->getObjects().at(0)->rotateBy(0.1f);
    // }
}

void ZRenderer::init() {
    mShader->use();
    mShader->setInt("irradianceMap", 0);
    mShader->setInt("prefilterMap", 1);
    mShader->setInt("brdfLUT", 2);

    glGenFramebuffers(1, &mMainFBOMS);
    glBindFramebuffer(GL_FRAMEBUFFER, mMainFBOMS);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  
    glGenTextures(1, &mMainBufferMS);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mMainBufferMS);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mSamples, GL_RGBA16F, mCamera->getWidth(), mCamera->getHeight(), GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenRenderbuffers(1, &mRenderBufferMS);
    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufferMS);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSamples, GL_DEPTH_COMPONENT, mCamera->getWidth(), mCamera->getHeight());

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mMainBufferMS, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBufferMS);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Main Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &mMainFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mMainFBO);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBufferMS);
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
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glLineWidth(2.0);

}

void ZRenderer::setRenderToTexture(bool toTexture) {
    mRenderToTexture = toTexture;
}

void ZRenderer::setWireMode(bool wiremode) {
    mWireMode = wiremode;
}

void ZRenderer::draw() {
    if (mScene != nullptr) {
        if (mParentView->getVisibility()) {


            updateAnimations();
            sortObjects();
            renderMain();
            renderSelection();
            renderToScreen();
            
          
            glDisable(GL_DEPTH_TEST);
        }
    }
}

void ZRenderer::sortObjects() {
    vector<ZObject*> objects = mScene->getObjects();
    vector<ZObject*> transparentObjects;
    mSortedObjects.clear();
    mSortedIndicies.clear();
    vector<int> transparentIndicies;

    int count = 0;
    for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
        ZObject *object = (*it);
        ZMaterial* material = object->getMaterial();
        if (material->getColor().a < 0.5) {
            transparentObjects.push_back(object);
            transparentIndicies.push_back(count);
        } else {
            mSortedObjects.push_back(object);
            mSortedIndicies.push_back(count);
        }
        count++;
    }

    mSortedObjects.insert(mSortedObjects.end(), transparentObjects.begin(), transparentObjects.end());
    mSortedIndicies.insert(mSortedIndicies.end(), transparentIndicies.begin(), transparentIndicies.end());
}

void ZRenderer::updateAnimations() {
    vector<ZObject*> objects = mScene->getObjects();
    for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
        ZObject *object = (*it);
        vector<ZAnimator*> animators = object->getAnimators();
        for (vector<ZAnimator*>::iterator it2 = animators.begin() ; it2 != animators.end(); ++it2) {
            ZAnimator* animator = (*it2);

            vec4 currentValue;
            vec4 desiredValue = animator->getDesiredValue();
            vec3 newTranslation;
            vec3 newRotation;
            float newRotationAngle;
            vec3 newScale;
            float speed = 300.0;
            int finished = 0;
            switch (animator->getType()) {
                case ZAnimator::rotation:
                    newRotation = object->getRotation();
                    newRotationAngle = object->getRotationAngle();
                    currentValue = vec4(object->getRotation(), object->getRotationAngle());
                    if (currentValue.x != desiredValue.x) {
                        float dif = currentValue.x - desiredValue.x;
                        newRotation.x -= dif / speed;
                        if (abs(dif) < 0.01) {
                            currentValue.x = desiredValue.x;
                            finished++;
                        }
                    }

                    if (currentValue.y != desiredValue.y) {
                        float dif = currentValue.y - desiredValue.y;
                        newRotation.y -= dif / speed;
                        if (abs(dif) < 0.01) {
                            currentValue.y = desiredValue.y;
                            finished++;
                        }
                    }

                    if (currentValue.z != desiredValue.z) {
                        float dif = currentValue.z - desiredValue.z;
                        newRotation.z -= dif / speed;
                        if (abs(dif) < 0.01) {
                            currentValue.z = desiredValue.z;
                            finished++;
                        }
                    }

                    if (currentValue.w != desiredValue.w) {
                        float dif = currentValue.w - desiredValue.w;
                        newRotationAngle -= dif / speed;
                        if (abs(dif) < 0.01) {
                            currentValue.w = desiredValue.w;
                            finished++;
                        }
                    }

                    if (finished != 4) {
                        object->setRotation(newRotation);
                        object->setRotationAngle(newRotationAngle);
                        mParentView->invalidate();
                    } else {
                        object->animationFinished(animator);
                    }
                    break;
                case ZAnimator::translation:
                    newTranslation = object->getTranslation();
                    currentValue = vec4(object->getTranslation(), 0);
                    if (currentValue.x != desiredValue.x) {
                        float dif = currentValue.x - desiredValue.x;
                        newTranslation.x -= dif / speed;
                        if (abs(dif) < 0.01) {
                            currentValue.x = desiredValue.x;
                            finished++;
                        }
                    }

                    if (currentValue.y != desiredValue.y) {
                        float dif = currentValue.y - desiredValue.y;
                        newTranslation.y -= dif / speed;
                        if (abs(dif) < 0.01) {
                            currentValue.y = desiredValue.y;
                            finished++;
                        }
                    }

                    if (currentValue.z != desiredValue.z) {
                        float dif = currentValue.z - desiredValue.z;
                        newTranslation.z -= dif / speed;
                        if (abs(dif) < 0.01) {
                            currentValue.z = desiredValue.z;
                            finished++;
                        }
                    }

                    if (finished != 3) {
                        object->setTranslation(newTranslation);
                        mParentView->invalidate();
                    } else {
                        object->animationFinished(animator);
                    }
                    break;
                case ZAnimator::scale:
                    newScale = object->getScale();
                    currentValue = vec4(object->getScale(), 0);
                    if (currentValue.x != desiredValue.x) {
                        float dif = currentValue.x - desiredValue.x;
                        newScale.x -= dif / speed;
                        if (abs(dif) < 0.01) {
                            currentValue.x = desiredValue.x;
                            finished++;
                        }
                    }

                    if (currentValue.y != desiredValue.y) {
                        float dif = currentValue.y - desiredValue.y;
                        newScale.y -= dif / speed;
                        if (abs(dif) < 0.01) {
                            currentValue.y = desiredValue.y;
                            finished++;
                        }
                    }

                    if (currentValue.z != desiredValue.z) {
                        float dif = currentValue.z - desiredValue.z;
                        newScale.z -= dif / speed;
                        if (abs(dif) < 0.01) {
                            currentValue.z = desiredValue.z;
                            finished++;
                        }
                    }

                    if (finished != 3) {
                        object->setScale(newScale);
                        mParentView->invalidate();
                    } else {
                        object->animationFinished(animator);
                    }
                    break;
                default:
                    currentValue = vec4(object->getScale(), 0);
                    break;
            }
        }
    }
}

void ZRenderer::recreateBuffers() {
    float width = mCamera->getWidth();
    float height =  mCamera->getHeight();

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mMainBufferMS);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mSamples, GL_RGBA16F, width, height, GL_TRUE);

    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufferMS);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSamples, GL_DEPTH_COMPONENT, width, height);


    glBindTexture(GL_TEXTURE_2D, mMainBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, mSelectionBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glBindRenderbuffer(GL_RENDERBUFFER, mSelectionRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

unsigned int ZRenderer::getMainTexture() {
    return mFinalBuffer;
}


void ZRenderer::renderMain() {
    float width = mCamera->getWidth();
    float height =  mCamera->getHeight();

    // Render to 16 bit frame buffer
    glViewport(0,0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, mMainFBOMS);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // Draw background
    mBackgroundShader->use();
    mBackgroundShader->setMat4("projection", mCamera->getProjectionMatrix());
    mBackgroundShader->setMat4("view", ZRenderUtils::getViewMatrix(mCamera));
    mBackgroundShader->setVec3("uColorFactor", mScene->getWorld()->getColor());

    glDisable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0);
    if (mScene->getWorld()->isBackgroundBlurred()) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, mScene->getWorld()->getIrradienceID());
    } else {
        glBindTexture(GL_TEXTURE_CUBE_MAP, mScene->getWorld()->getBackgroundID());
    }

    if (!mCamera->isPerspective()) {
        mCamera->setUsePerspective(true);
        renderCube();
        mCamera->setUsePerspective(false);
    } else {
        renderCube();
    }

    if (mWireMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glEnable(GL_CULL_FACE);

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
    shader->setMat4("uViewMatrix", ZRenderUtils::getViewMatrix(mCamera));

    int mPositionLocation = glGetAttribLocation(shader->mID, "aPos");
    int mNormalLocation = glGetAttribLocation(shader->mID, "aNormal");
    int mTextureCoordLocation = glGetAttribLocation(shader->mID, "aTextureCoords");

    mat4 identityMatrix = mat4();
    shader->setMat4("uModelMatrix", identityMatrix);

    int objectIndex = 0;
    for (vector<ZObject*>::iterator it = mSortedObjects.begin() ; it != mSortedObjects.end(); ++it) {
        ZObject *object = (*it);
        ZMesh *mesh = (*it)->getMesh();
        ZMaterial* material = object->getMaterial();

        shader->setMat4("uModelMatrix", ZRenderUtils::getModelMatrix(object, mCamera));

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

        if (mWireMode) {
            shader->setVec4("uColor", 0.0, 0.0, 0.0, 1.0);
        } else {
            shader->setVec4("uColor", color.r, color.g, color.b, color.a);
        }


        float selected = 0;
        if (mScene->getActiveObjectIndex() == mSortedIndicies.at(objectIndex)) {
            selected = 1;
        }
        shader->setFloat("uSelected", selected);

        if (mWireMode) {
            shader->setFloat("uMetallic", 0); 
            shader->setFloat("uRoughness", 1);
        } else {
            shader->setFloat("uMetallic", material->getMetallic());
            shader->setFloat("uRoughness", material->getRoughness());
        }

        if (mCamera->isManualView()) {
            shader->setVec3("uCameraPosition", inverse(mCamera->getViewMatrix()) * vec4(0,0,0,1));
        } else {
            if (mCamera->isPerspective()) {
                shader->setVec3("uCameraPosition", ZRenderUtils::getModelMatrix(mCamera, nullptr) * vec4(0,0,0,1));
            } else {
             shader->setVec3("uCameraPosition", ZRenderUtils::getModelMatrix(mCamera, nullptr, vec3(0,0,50)) * vec4(0,0,0,1));
            }
       
        }

        if (object->getVisible()) {
            if (material->getColor().a < 0.5) {
                glDepthMask(false);
            }
            glDrawElements(GL_TRIANGLES, mesh->getFaceIndiceCount(), GL_UNSIGNED_INT, nullptr); 
            glDepthMask(true);
        }   
        objectIndex++;
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, mMainFBOMS);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mMainFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);


    onDrawFinshed();
}

void ZRenderer::renderSelection() {
    //if (!mRenderToTexture) {
        glBindFramebuffer(GL_FRAMEBUFFER, mSelectionFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mSelectionShader->use();
        mSelectionShader->setMat4("uProjectionMatrix", mCamera->getProjectionMatrix());
        mSelectionShader->setMat4("uViewMatrix", ZRenderUtils::getViewMatrix(mCamera));

        vector<ZObject*> objects = mScene->getObjects();

        int mPositionLocation = glGetAttribLocation(mSelectionShader->mID, "aPos");
        int mNoramlLocation = glGetAttribLocation(mSelectionShader->mID, "aNormal");
   
        vector<ZObject*> transparentObjects;
        vector<ZObject*> solidObjects;
        int count = 0;
        for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
            ZObject *object = (*it);
            ZMaterial* material = object->getMaterial();
            if (material->getColor().a < 0.5) {
                transparentObjects.push_back(object);
            } else {
                solidObjects.push_back(object);
            }
            count++;
        }

        int objectIndex = 0;
        for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {

            ZMesh *mesh = (*it)->getMesh();

            mSelectionShader->setMat4("uModelMatrix", ZRenderUtils::getModelMatrix((*it), mCamera));

            glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexBuffer());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getFaceIndicesBuffer());

            glEnableVertexAttribArray(mPositionLocation);
            glVertexAttribPointer(mPositionLocation, 3, GL_FLOAT, GL_FALSE,
              sizeof(float) * 3, (void*) 0);

            glEnableVertexAttribArray(mNoramlLocation);
            glVertexAttribPointer(mNoramlLocation, 3, GL_FLOAT, GL_FALSE,
              sizeof(float) * 3, (void*) 0);

            vec4 color = vec4((float) objectIndex / 256,0,0,1.0);
            mSelectionShader->setVec4("uColor", color.r, color.g, color.b, color.a);

            if ((*it)->getSelectable() && (*it)->getVisible()) {
                glDrawElements(GL_TRIANGLES, mesh->getFaceIndiceCount(), GL_UNSIGNED_INT, nullptr); 
            }

            objectIndex++;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    //}
}

int ZRenderer::getMainBuffer() {
    return mMainBuffer;
}

void ZRenderer::renderToScreen() {
    // if (mRenderToTexture) {
    //     glBindFramebuffer(GL_FRAMEBUFFER, mFinalFBO);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //     mHDRShader->use();
    //     glViewport(0,0,mCamera->getWidth(),mCamera->getHeight());
    //     glActiveTexture(GL_TEXTURE0);
    //     glBindTexture(GL_TEXTURE_2D, mMainBuffer);

    //     mHDRShader->setBool("hdr", true);
    //     mHDRShader->setFloat("exposure", mScene->getExposure());

    //     renderQuad();
    //     glBindTexture(GL_TEXTURE_2D, 0);
    // } else {

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_DEPTH_BUFFER_BIT);
      
        mHDRShader->use();

        glEnable(GL_DITHER);
       
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mMainBuffer);
        mHDRShader->setBool("hdr", true);
        mHDRShader->setFloat("exposure", mScene->getExposure());

        int yv = mParentView->getWindowHeight() - mParentView->getBottom();
        glViewport(mParentView->getLeft(),yv,mParentView->getWidth(),mParentView->getHeight());
    
        renderQuad();
        glBindTexture(GL_TEXTURE_2D, 0);

   // }
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

void ZRenderer::setCamera(ZCamera* camera) {
    mCamera = camera;
}

void ZRenderer::renderQuad() {
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -0.0f, -0.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -0.0f, 0.0f, 1.0f, 0.0f,
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

void ZRenderer::onExit() {

    glDeleteRenderbuffers( 1, &mRenderBufferMS);
    glDeleteTextures( 1, &mMainBufferMS);
    glDeleteFramebuffers( 1, &mMainFBOMS);

    glDeleteRenderbuffers( 1, &mSelectionRenderBuffer);
    glDeleteTextures( 1, &mSelectionBuffer);
    glDeleteFramebuffers( 1, &mSelectionFBO);

    mHDRShader->deleteProgram();
    mSelectionShader->deleteProgram();
    mShader->deleteProgram();

cout << "exit renderer" << endl;
}

unsigned int ZRenderer::getMainFBO() {
    return mMainFBOMS;
}

void ZRenderer::setAASamples(int samples) {
    mSamples = samples;
}
