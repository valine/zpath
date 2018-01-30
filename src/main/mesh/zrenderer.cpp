#include "zrenderer.h"

ZRenderer::ZRenderer(string resourcePath) {

	string vertexPath = resourcePath + "resources/shaders/generalvertexshader.glsl";
    string fragmentPath = resourcePath + "resources/shaders/generalfragmentshader.glsl";
    mShader = new ZShader(vertexPath, fragmentPath);

	mPositionLocation = glGetAttribLocation(mShader->mID, "vPos");
	mNormalLocation = glGetAttribLocation(mShader->mID, "vNormal");

	mColorLocation = glGetUniformLocation(mShader->mID, "uColor");
	
	mViewMatrixLocation = glGetUniformLocation(mShader->mID, "uViewMatrix");
	mProjectionMatrixLocation = glGetUniformLocation(mShader->mID, "uProjectionMatrix");

	glDepthFunc(GL_LEQUAL);

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

			glProgramUniform4f(mShader->mID, mColorLocation,
			    1, 0, 
			    0, 1);


			float width = 700;
			float height = 701;
			mat4 projectionMatrix = perspective(glm::radians(35.0f), (float) width / (float) height, 0.1f, 100.0f);
			
			mat4 viewMatrix = glm::lookAt(
			    glm::vec3(5,0,0), // Camera is at (4,3,3), in World Space
			    glm::vec3(0,0,0), // and looks at the origin
			    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
			);

			viewMatrix = glm::rotate(viewMatrix, (float)glfwGetTime(), glm::vec3(0, 1, 0)); // where x, y, z is axis of rotation (e.g. 0 1 0)

			glUniformMatrix4fv(mProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glUniformMatrix4fv(mViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		

			glDrawElements(GL_TRIANGLES, mesh->getFaceIndiceCount(), GL_UNSIGNED_INT, nullptr); 
	    }

	    glDisable(GL_DEPTH_TEST);
	}
}

void ZRenderer::setScene(ZScene *scene) {
	mScene = scene;
}