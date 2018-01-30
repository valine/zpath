#include "zmesh.h"

void ZMesh::setVertices(vector<float> vertices) {
	mVertices = vertices;

    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), &mVertices[0], GL_STATIC_DRAW);
	
}

void ZMesh::setFaceIndices(vector<int> indices) {
	mFaceIndices = indices;

    glGenBuffers(1, &mFaceIndicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mFaceIndices.size() * sizeof(int), &mFaceIndices[0], GL_STATIC_DRAW);
}

vector<float> ZMesh::getVertices() const {
	return mVertices;
}

vector<int> ZMesh::getFaceIndices() const {
	return mFaceIndices;
}

GLuint ZMesh::getVertexBuffer() {
	return mVertexBuffer;
}

GLuint ZMesh::getFaceIndicesBuffer() {
	return mFaceIndicesBuffer;
}

