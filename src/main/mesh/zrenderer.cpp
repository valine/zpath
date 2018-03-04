#include "zrenderer.h"

ZRenderer::ZRenderer(string resourcePath) {

	string vertexPath = resourcePath + "resources/shaders/generalvertexshader.glsl";
    string fragmentPath = resourcePath + "resources/shaders/generalfragmentshader.glsl";
    mShader = new ZShader(vertexPath, fragmentPath);
	
	string fboVertexPath = resourcePath + "resources/shaders/fbovertex.glsl";
    string fboFragmentPath = resourcePath + "resources/shaders/fbofragment.glsl";
    mHDRShader = new ZShader(fboVertexPath, fboFragmentPath);

	mPositionLocation = glGetAttribLocation(mShader->mID, "aPos");
	mNormalLocation = glGetAttribLocation(mShader->mID, "aNormal");
	mColorLocation = glGetUniformLocation(mShader->mID, "uColor");
	mCameraPositionLocation = glGetUniformLocation(mShader->mID, "uCameraPosition");

	mViewMatrixLocation = glGetUniformLocation(mShader->mID, "uViewMatrix");
	mProjectionMatrixLocation = glGetUniformLocation(mShader->mID, "uProjectionMatrix");

	glDepthFunc(GL_LEQUAL);
	
	mCamera = new ZCamera();

}

void ZRenderer::init() {
	glGenFramebuffers(1, &mHdrFBO);

	glGenTextures(1, &mColorBuffer);
    glBindTexture(GL_TEXTURE_2D, mColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mParentView->getWidth(), mParentView->getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

 	glGenRenderbuffers(1, &mRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mParentView->getWidth(), mParentView->getHeight());


    glBindFramebuffer(GL_FRAMEBUFFER, mHdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    mHDRShader->use();
    glUniform1i(glGetUniformLocation(mHDRShader->mID, "hdrBuffer"), 0); 

}

void ZRenderer::draw() {
	if (mScene != nullptr) {

		float width = mParentView->getWidth();
		float height =  mParentView->getHeight();

	   	glBindTexture(GL_TEXTURE_2D, mColorBuffer);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
	    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);


		// Render to 16 bit frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, mHdrFBO);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f );
 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 		glViewport(0,0, width, height);
		mShader->use();
		glEnable(GL_DEPTH_TEST);

		vector<ZObject*> objects = mScene->getObjects();
		for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
			ZObject *object = (*it);
	    	ZMesh *mesh = (*it)->getMesh();

	    	glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexBuffer());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getFaceIndicesBuffer());

			glEnableVertexAttribArray(mPositionLocation);
			glVertexAttribPointer(mPositionLocation, 3, GL_FLOAT, GL_FALSE,
			                          sizeof(float) * 3, (void*) 0);

			glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexNormalBuffer());

			glEnableVertexAttribArray(mNormalLocation);
			glVertexAttribPointer(mNormalLocation, 3, GL_FLOAT, GL_FALSE,
			                          sizeof(float) * 3, (void*) 0);


			vec3 color = object->getColor();
			glUniform4f(mColorLocation,
			    color.r, color.g, 
			    color.b, 1);

			vec3 cameraPos = mCamera->getPosition();
			glUniform3f(mCameraPositionLocation, cameraPos.x, cameraPos.y, cameraPos.z);

			if (mParentView != nullptr) {
				float width = mParentView->getWidth();
				float height = mParentView->getHeight();

				mat4 projectionMatrix = perspective(glm::radians(35.0f), (float) width / (float) height, 0.1f, 100.0f);

				glUniformMatrix4fv(mProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
				glUniformMatrix4fv(mViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mCamera->getViewMatrix()));
			
				glDrawElements(GL_TRIANGLES, mesh->getFaceIndiceCount(), GL_UNSIGNED_INT, nullptr); 
			}
	    }

	    glBindFramebuffer(GL_FRAMEBUFFER, 0);

         mHDRShader->use();


		int yv = mParentView->getWindowHeight() - mParentView->getBottom();
		glViewport(mParentView->getLeft(),yv,mParentView->getWidth(),mParentView->getHeight());


         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, mColorBuffer);

         glUniform1i(glGetUniformLocation(mHDRShader->mID, "hdr"), true); 
         glUniform1f(glGetUniformLocation(mHDRShader->mID, "exposure"), mScene->getExposure()); 

         renderQuad();


	    glDisable(GL_DEPTH_TEST);
	}
}

void ZRenderer::setScene(ZScene *scene) {
	mScene = scene;
}

void ZRenderer::setParentView(Z3DView* view) {
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