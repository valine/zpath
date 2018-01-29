#include "z3dview.h"

/**
	Z3DView constructor 
*/
Z3DView::Z3DView(float maxWidth, float maxHeight, string resourcePath) 
: ZView(maxWidth, maxHeight) {

	ZObjLoader loader = ZObjLoader("");
	ZMesh mesh = loader.loadMesh("");

    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.getVertices().size() * sizeof(float), &mesh.getVertices()[0], GL_STATIC_DRAW);

    glGenBuffers(1, &mFaceIndicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getFaceIndices().size() * sizeof(int), &mesh.getFaceIndices()[0], GL_STATIC_DRAW);
	
	string vertexPath = resourcePath + "resources/shaders/generalvertexshader.glsl";
    string fragmentPath = resourcePath + "resources/shaders/generalfragmentshader.glsl";
    mShader = new ZShader(vertexPath, fragmentPath);

    mPositionLocation = glGetAttribLocation(mShader->mID, "vPos");
    mColorLocation = glGetUniformLocation(mShader->mID, "uColor");
}

void Z3DView::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);

	
}

void Z3DView::onKeyPress(int key, int scancode, int action, int mods) {

}
	
void Z3DView::onCursorPosChange(double x, double y) {
	if (mouseIsDown()) {
	
    }
}

void Z3DView::setRenderer(ZRenderer renderer) {
    mRenderer = renderer;
}

void Z3DView::draw() {
	mShader->use();

	int y = getParentView()->getHeight() + getParentView()->getTop() - getTop() - getHeight();
	glViewport(getLeft(),y,getWidth(),getHeight());
	
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);

    glEnableVertexAttribArray(mPositionLocation);
    glVertexAttribPointer(mPositionLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 3, (void*) 0);

     glProgramUniform4f(mShader->mID, mColorLocation,
        1, 0, 
        0, 1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); 
}