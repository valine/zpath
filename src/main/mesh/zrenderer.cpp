#include "zrenderer.h"

ZRenderer::ZRenderer(string resourcePath) {

	string vertexPath = resourcePath + "resources/shaders/baseVertex.glsl";
    string fragmentPath = resourcePath + "resources/shaders/baseFragment.glsl";
    mShader = new ZShader(vertexPath, fragmentPath);

    string colorTextureVertexPath = resourcePath + "resources/shaders/baseVertex.glsl";
    string colorTextureFragmentPath = resourcePath + "resources/shaders/colorTextureFragment.glsl";
    mColorTextureShader = new ZShader(colorTextureVertexPath, colorTextureFragmentPath);
	
	string fboVertexPath = resourcePath + "resources/shaders/fbovertex.glsl";
    string fboFragmentPath = resourcePath + "resources/shaders/fbofragment.glsl";
    mHDRShader = new ZShader(fboVertexPath, fboFragmentPath);

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
    mHDRShader->setInt("hdrBuffer", 0);
}

// Experimental features
void ZRenderer::onDrawFinshed() {
	//renderQuad();

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
		glEnable(GL_DEPTH_TEST);

		vector<ZPointLight*> lights = mScene->getLights();

		vector<ZObject*> objects = mScene->getObjects();
		for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
			ZObject *object = (*it);
	    	ZMesh *mesh = (*it)->getMesh();
	    	ZMaterial* material = object->getMaterial();

	    	ZShader* shader;

	    	if (material->getColorTexture() != nullptr) {
	    		shader = mColorTextureShader;
	    	} else {
	    		shader = mShader;
	    	}

	    	shader->use();
			
			shader->setVec3("uLightPositions", (uint) lights.size(), mScene->getLightPositions());
			shader->setVec3("uLightColors", (uint) lights.size(), mScene->getLightColors());

	    	glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexBuffer());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getFaceIndicesBuffer());

			int mPositionLocation = glGetAttribLocation(shader->mID, "aPos");
			int mNormalLocation = glGetAttribLocation(shader->mID, "aNormal");

			glEnableVertexAttribArray(mPositionLocation);
			glVertexAttribPointer(mPositionLocation, 3, GL_FLOAT, GL_FALSE,
			                          sizeof(float) * 3, (void*) 0);

			glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexNormalBuffer());

			glEnableVertexAttribArray(mNormalLocation);
			glVertexAttribPointer(mNormalLocation, 3, GL_FLOAT, GL_FALSE,
			                          sizeof(float) * 3, (void*) 0);


	    	vec3 color = material->getColor();
			shader->setVec4("uColor", color.r, color.g, color.b, 1);

	    	shader->setFloat("uMetallic", material->getMetallic());
			shader->setFloat("uRoughness", material->getRoughness());
			shader->setVec3("uCameraPosition", mCamera->getPosition());
			
			if (mParentView != nullptr) {
				float width = mParentView->getWidth();
				float height = mParentView->getHeight();

				mat4 projectionMatrix = perspective(glm::radians(35.0f), (float) width / (float) height, 0.1f, 100.0f);

				shader->setMat4("uProjectionMatrix", projectionMatrix);
				shader->setMat4("uViewMatrix", mCamera->getViewMatrix());
			
				glDrawElements(GL_TRIANGLES, mesh->getFaceIndiceCount(), GL_UNSIGNED_INT, nullptr); 
			}
	    }

	    onDrawFinshed();

	    glBindFramebuffer(GL_FRAMEBUFFER, 0);

        mHDRShader->use();
		int yv = mParentView->getWindowHeight() - mParentView->getBottom();
		glViewport(mParentView->getLeft(),yv,mParentView->getWidth(),mParentView->getHeight());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mColorBuffer);

        mHDRShader->setBool("hdr", true);
        mHDRShader->setFloat("exposure", mScene->getExposure());
    
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