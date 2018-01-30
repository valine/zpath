#include "zmesh.h"

void ZMesh::setVertices(vector<float> vertices) {
	mVertices = vertices;

    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), &mVertices[0], GL_STATIC_DRAW);
	
}

void ZMesh::setVertexNormals(vector<float> normals) {
	mVertexNormals = normals;

    glGenBuffers(1, &mVertexNormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVertexNormals.size() * sizeof(float), &mVertexNormals[0], GL_STATIC_DRAW);
	
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

vector<float> ZMesh::getVertexNormals() const {
	return mVertexNormals;
}


vector<int> ZMesh::getFaceIndices() const {
	return mFaceIndices;
}

GLuint ZMesh::getVertexBuffer() {
	return mVertexBuffer;
}

GLuint ZMesh::getVertexNormalBuffer() {
	return mVertexNormalBuffer;
}


GLuint ZMesh::getFaceIndicesBuffer() {
	return mFaceIndicesBuffer;
}

int ZMesh::getFaceIndiceCount() {
	return mFaceIndices.size();
}

