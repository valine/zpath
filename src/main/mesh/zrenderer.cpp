#include "zrenderer.h"

ZRenderer::ZRenderer(string resourcePath) {

	string vertexPath = resourcePath + "resources/shaders/generalvertexshader.glsl";
    string fragmentPath = resourcePath + "resources/shaders/generalfragmentshader.glsl";
    mShader = new ZShader(vertexPath, fragmentPath);

	mPositionLocation = glGetAttribLocation(mShader->mID, "vPos");
	mColorLocation = glGetUniformLocation(mShader->mID, "uColor");

}

void ZRenderer::draw() {
	if (mScene != nullptr) {
		mShader->use();

		vector<ZObject*> objects = mScene->getObjects();
		for (vector<ZObject*>::iterator it = objects.begin() ; it != objects.end(); ++it) {
	    	ZMesh *mesh = (*it)->getMesh();

	    	glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexBuffer());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getFaceIndicesBuffer());

			glEnableVertexAttribArray(mPositionLocation);
			glVertexAttribPointer(mPositionLocation, 3, GL_FLOAT, GL_FALSE,
			                          sizeof(float) * 3, (void*) 0);

			glProgramUniform4f(mShader->mID, mColorLocation,
			    1, 0, 
			    0, 1);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); 
	    }
	}
}

void ZRenderer::setScene(ZScene *scene) {
	mScene = scene;
}