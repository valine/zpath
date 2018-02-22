#include "zrenderer.h"

ZRenderer::ZRenderer(string resourcePath) {

	string vertexPath = resourcePath + "resources/shaders/generalvertexshader.glsl";
    string fragmentPath = resourcePath + "resources/shaders/generalfragmentshader.glsl";
    mShader = new ZShader(vertexPath, fragmentPath);

	mPositionLocation = glGetAttribLocation(mShader->mID, "aPos");
	mNormalLocation = glGetAttribLocation(mShader->mID, "aNormal");
	mColorLocation = glGetUniformLocation(mShader->mID, "uColor");

	mViewMatrixLocation = glGetUniformLocation(mShader->mID, "uViewMatrix");
	mProjectionMatrixLocation = glGetUniformLocation(mShader->mID, "uProjectionMatrix");

	glDepthFunc(GL_LEQUAL);
	
	mCamera = new ZCamera();

}

void ZRenderer::draw() {
	if (mScene != nullptr) {
		mShader->use();
		glEnable(GL_DEPTH_TEST);

		vector<ZObject*> objects = mScene->getObjects();
		for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
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

			glUniform4f(mColorLocation,
			    1, 0, 
			    0, 1);


			if (mParentView != nullptr) {
				float width = mParentView->getWidth();
				float height = mParentView->getHeight();

				mat4 projectionMatrix = perspective(glm::radians(35.0f), (float) width / (float) height, 0.1f, 100.0f);

				glUniformMatrix4fv(mProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
				glUniformMatrix4fv(mViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mCamera->getViewMatrix()));
			
				glDrawElements(GL_TRIANGLES, mesh->getFaceIndiceCount(), GL_UNSIGNED_INT, nullptr); 
			}
	    }

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