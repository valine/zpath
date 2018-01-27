#include "z3dview.h"

/**
	Z3DView constructor 
*/
Z3DView::Z3DView(float maxWidth, float maxHeight, string resourcePath) 
: ZView(maxWidth, maxHeight) {

	mVertices.push_back(0.0);
	mVertices.push_back(0.0);
	mVertices.push_back(0.0);


	mVertices.push_back(1.0);
	mVertices.push_back(0.0);
	mVertices.push_back(0.0);

	mVertices.push_back(1.0);
	mVertices.push_back(1.0);
	mVertices.push_back(0.0);

	mFaceIndices.push_back(0);
	mFaceIndices.push_back(1);
	mFaceIndices.push_back(2);


    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), &mVertices.front(), GL_STATIC_DRAW);

    glGenBuffers(1, &mFaceIndicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mFaceIndices) * sizeof(int), &mFaceIndices.front(), GL_STATIC_DRAW);
	
	string vertexPath = resourcePath + "resources/shaders/generalvertexshader.glsl";
    string fragmentPath = resourcePath + "resources/shaders/generalfragmentshader.glsl";
    mShader = new ZShader(vertexPath, fragmentPath);


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

void Z3DView::draw() {
	mShader->use();
	glViewport(getLeft(),getRight(),getTop(),getBottom());
}