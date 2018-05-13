#include "zrenderer.h"

ZRenderer::ZRenderer(string resourcePath) {
	string vertexPath = resourcePath + "resources/shaders/base.vs";
    string fragmentPath = resourcePath + "resources/shaders/base.fs";
    mShader = new ZShader(vertexPath, fragmentPath);

    string backgroundVertexPath = resourcePath + "resources/shaders/background.vs";
    string backgroundFragmentPath = resourcePath + "resources/shaders/background.fs";
    mBackgroundShader = new ZShader(backgroundVertexPath, backgroundFragmentPath);

    string colorTextureVertexPath = resourcePath + "resources/shaders/base.vs";
    string colorTextureFragmentPath = resourcePath + "resources/shaders/colorTexture.fs";
    mColorTextureShader = new ZShader(colorTextureVertexPath, colorTextureFragmentPath);

    string selectionVertexPath = resourcePath + "resources/shaders/selection.vs";
    string selectionFragmentPath = resourcePath + "resources/shaders/selection.fs";
    mSelectionShader = new ZShader(selectionVertexPath, selectionFragmentPath);

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

	glGenFramebuffers(1, &mMainFBO);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  
	glGenTextures(1, &mMainBuffer);
    glBindTexture(GL_TEXTURE_2D, mMainBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mParentView->getWidth(), mParentView->getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

 	glGenRenderbuffers(1, &mRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mParentView->getWidth(), mParentView->getHeight());

    glBindFramebuffer(GL_FRAMEBUFFER, mMainFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mMainBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Create selection offscreen buffer
	glGenFramebuffers(1, &mSelectionFBO);
	glGenTextures(1, &mSelectionBuffer);
    glBindTexture(GL_TEXTURE_2D, mSelectionBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mParentView->getWidth(), mParentView->getHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

 	glGenRenderbuffers(1, &mSelectionRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mSelectionRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mParentView->getWidth(), mParentView->getHeight());

    glBindFramebuffer(GL_FRAMEBUFFER, mSelectionFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSelectionBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mSelectionRenderBuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    mHDRShader->use();
    mHDRShader->setInt("hdrBuffer", 0);

    // Create cube map
    if (mScene->getWorld()->getEnvironmentTexture() != nullptr) {

	    // Transparency setup
	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	    glBlendEquation (GL_FUNC_ADD);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

// Experimental features
void ZRenderer::onDrawFinshed() {
	//renderQuad();
	//renderCube();
}

void ZRenderer::draw() {
	if (mScene != nullptr) {

		if (mParentView->getVisibility()) {

			float width = mParentView->getWidth();
			float height =  mParentView->getHeight();

			mCamera->setWidth(width);
			mCamera->setHeight(height);

		   	glBindTexture(GL_TEXTURE_2D, mMainBuffer);
		    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		    
		    glBindTexture(GL_TEXTURE_2D, mSelectionBuffer);
		    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

		    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
		    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

			renderMain();
			renderSelection();
		   	renderToScreen();

		    glDisable(GL_DEPTH_TEST);
		}
	}
}

void ZRenderer::renderMain() {
	float width = mParentView->getWidth();
	float height =  mParentView->getHeight();

	// Render to 16 bit frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, mMainFBO);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0,0, width, height);
	glEnable(GL_DEPTH_TEST);

	// Draw background
	mBackgroundShader->use();
	mBackgroundShader->setMat4("projection", mCamera->getProjectionMatrix());
	mBackgroundShader->setMat4("view", mCamera->getViewMatrix());
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

	shader->setVec3("uLightPositions", (uint) lights.size(), mScene->getLightPositions());
	shader->setVec3("uLightColors", (uint) lights.size(), mScene->getLightColors());

	shader->setMat4("uProjectionMatrix", mCamera->getProjectionMatrix());
	shader->setMat4("uViewMatrix", mCamera->getViewMatrix());

	int mPositionLocation = glGetAttribLocation(shader->mID, "aPos");
	int mNormalLocation = glGetAttribLocation(shader->mID, "aNormal");
	int mTextureCoordLocation = glGetAttribLocation(shader->mID, "aTextureCoords");

	int objectIndex = 0;
	for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
		ZObject *object = (*it);
    	ZMesh *mesh = (*it)->getMesh();
    	ZMaterial* material = object->getMaterial();

    	
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
	glBindFramebuffer(GL_FRAMEBUFFER, mSelectionFBO);

	mSelectionShader->use();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mSelectionShader->setMat4("uProjectionMatrix", mCamera->getProjectionMatrix());
	mSelectionShader->setMat4("uViewMatrix", mCamera->getViewMatrix());

	vector<ZObject*> objects = mScene->getObjects();

	int mPositionLocation = glGetAttribLocation(mSelectionShader->mID, "aPos");

	int objectIndex = 0;
	for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
    	ZMesh *mesh = (*it)->getMesh();

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

void ZRenderer::renderToScreen() {

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
