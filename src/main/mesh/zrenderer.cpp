#include "zrenderer.h"

ZRenderer::ZRenderer(string resourcePath) {

	string vertexPath = resourcePath + "resources/shaders/base.vs";
    string fragmentPath = resourcePath + "resources/shaders/base.fs";
    mShader = new ZShader(vertexPath, fragmentPath);


	string cubeVertexPath = resourcePath + "resources/shaders/cubemap.vs";
    string cubeFragmentPath = resourcePath + "resources/shaders/cubemap.fs";
    mCubemapShader = new ZShader(cubeVertexPath, cubeFragmentPath);


    string backgroundVertexPath = resourcePath + "resources/shaders/background.vs";
    string backgroundFragmentPath = resourcePath + "resources/shaders/background.fs";
    mBackgroundShader = new ZShader(backgroundVertexPath, backgroundFragmentPath);


    string colorTextureVertexPath = resourcePath + "resources/shaders/base.vs";
    string colorTextureFragmentPath = resourcePath + "resources/shaders/colorTexture.fs";
    mColorTextureShader = new ZShader(colorTextureVertexPath, colorTextureFragmentPath);

    string prefTextureVertexPath = resourcePath + "resources/shaders/cubemap.vs";
    string prefTextureFragmentPath = resourcePath + "resources/shaders/prefilter.fs";
    mPrefilterShader = new ZShader(prefTextureVertexPath, prefTextureFragmentPath);

    string irradienceTextureVertexPath = resourcePath + "resources/shaders/cubemap.vs";
    string irradienceTextureFragmentPath = resourcePath + "resources/shaders/irradiance.fs";
    mIrradienceShader = new ZShader(irradienceTextureVertexPath, irradienceTextureFragmentPath);
	

    string brdfVertexPath = resourcePath + "resources/shaders/brdf.vs";
    string brdfFragmentPath = resourcePath + "resources/shaders/brdf.fs";
    mBrdfShader = new ZShader(brdfVertexPath, brdfFragmentPath);
	
	string fboVertexPath = resourcePath + "resources/shaders/fbo.vs";
    string fboFragmentPath = resourcePath + "resources/shaders/fbo.fs";
    mHDRShader = new ZShader(fboVertexPath, fboFragmentPath);

	glDepthFunc(GL_LEQUAL);
	
	mCamera = new ZCamera();

}

void ZRenderer::init() {

	mShader->use();
	mShader->setInt("irradianceMap", 0);
    mShader->setInt("prefilterMap", 1);
    mShader->setInt("brdfLUT", 2);

	glGenFramebuffers(1, &mHdrFBO);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  
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

    // Create cube map

    if (mScene->getWorld()->getEnvironmentTexture() != nullptr) {

	    unsigned int captureFBO;
	    unsigned int captureRBO;
	    glGenFramebuffers(1, &captureFBO);
	    glGenRenderbuffers(1, &captureRBO);

	    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048);
	    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	  

	  	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	    glm::mat4 captureViews[] = {
	
	    	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
	       	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),

	        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, 1.0f)),
	        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  -1.0f)),
	    
	    	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
	     	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, 1.0f,  0.0f))

	    };

	  	// Create background cube map
	  	glGenTextures(1, &envCubemap);
	    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	    for (unsigned int i = 0; i < 6; ++i)
	    {
	        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 2048, 2048, 0, GL_RGB, GL_FLOAT, nullptr);
	    }
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	    mCubemapShader->use();
	    mCubemapShader->setInt("equirectangularMap", 0);
	    mCubemapShader->setMat4("projection", captureProjection);
	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, mScene->getWorld()->getEnvironmentTexture()->getID());

	    glViewport(0, 0, 2048, 2048); // don't forget to configure the viewport to the capture dimensions.
	    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	    for (unsigned int i = 0; i < 6; ++i) {
	        mCubemapShader->setMat4("view", captureViews[i]);
	        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
	        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	        renderCube();
	    }

	    glBindFramebuffer(GL_FRAMEBUFFER, 0);

   		 // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	    
	    // Create irradiance map
	    glGenTextures(1, &irradienceCubemap);
	    glBindTexture(GL_TEXTURE_CUBE_MAP, irradienceCubemap);
	     for (unsigned int i = 0; i < 6; ++i) {
	        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	    }
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	    mIrradienceShader->use();
	    mIrradienceShader->setInt("equirectangularMap", 0);
	    mIrradienceShader->setMat4("projection", captureProjection);
	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	    glViewport(0, 0, 32, 32); 
	    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	    for (unsigned int i = 0; i < 6; ++i) {
	        mCubemapShader->setMat4("view", captureViews[i]);
	        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradienceCubemap, 0);
	        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	        renderCube();
	    }

	     glBindFramebuffer(GL_FRAMEBUFFER, 0);

	    // Generate pre-filtered environment map

	    glGenTextures(1, &prefilterMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
		    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 256, 256, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	    mPrefilterShader->use();
	    mPrefilterShader->setInt("environmentMap", 0);
	    mPrefilterShader->setMat4("projection", captureProjection);
	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	    unsigned int maxMipLevels = 5;
	    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	    {
	        // reisze framebuffer according to mip-level size.
	        unsigned int mipWidth  = 256 * std::pow(0.5, mip);
	        unsigned int mipHeight = 256 * std::pow(0.5, mip);
	        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
	        glViewport(0, 0, mipWidth, mipHeight);

	        float roughness = (float)mip / (float)(maxMipLevels - 1);
	        mPrefilterShader->setFloat("roughness", roughness);
	        for (unsigned int i = 0; i < 6; ++i)
	        {
	            mPrefilterShader->setMat4("view", captureViews[i]);
	            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

	            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	            renderCube();
	        }
	    }

	    glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenTextures(1, &brdfLUTTexture);

	    // pre-allocate enough memory for the LUT texture.
	    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	    glViewport(0, 0, 512, 512);
	    mBrdfShader->use();
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    renderQuad();

	    // Unbind texture
	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

// Experimental features
void ZRenderer::onDrawFinshed() {
	//renderQuad();
	//renderCube();

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


		mat4 projectionMatrix = perspective(glm::radians(35.0f), (float) width / (float) height, 0.1f, 1000.0f);

		vector<ZPointLight*> lights = mScene->getLights();

		vector<ZObject*> objects = mScene->getObjects();
		
	    glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradienceCubemap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

		for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
			ZObject *object = (*it);
	    	ZMesh *mesh = (*it)->getMesh();
	    	ZMaterial* material = object->getMaterial();

	    	ZShader* shader;

	    	if (material->getColorTexture() != nullptr) {
	    		shader = mColorTextureShader;
	    		glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, material->getColorTexture()->getID());
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
			int mTextureCoordLocation = glGetAttribLocation(shader->mID, "aTextureCoords");

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

	    	vec3 color = material->getColor();
			shader->setVec4("uColor", color.r, color.g, color.b, 1);

	    	shader->setFloat("uMetallic", material->getMetallic());
			shader->setFloat("uRoughness", material->getRoughness());
			shader->setVec3("uCameraPosition", mCamera->getPosition());
			
			if (mParentView != nullptr) {

				// float width = mParentView->getWidth();
				// float height = mParentView->getHeight();

				shader->setMat4("uProjectionMatrix", projectionMatrix);
				shader->setMat4("uViewMatrix", mCamera->getViewMatrix());
		
				glDrawElements(GL_TRIANGLES, mesh->getFaceIndiceCount(), GL_UNSIGNED_INT, nullptr); 
			}
	    }

	    glBindTexture(GL_TEXTURE_2D, 0);

	    mBackgroundShader->use();
    	mBackgroundShader->setMat4("projection", projectionMatrix);
    	mBackgroundShader->setMat4("view", mCamera->getViewMatrix());
    	glActiveTexture(GL_TEXTURE0);
       glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        renderCube();

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

void ZRenderer::renderCube() {
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
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
