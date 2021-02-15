#include <utility>

#include <utility>

#include "mesh/zmesh.h"


ZMesh::ZMesh() {
    glGenBuffers(1, &mVertexBuffer);
    glGenBuffers(1, &mVertexNormalBuffer);
    glGenBuffers(1, &mTextureCoordinatesBuffer);
    glGenBuffers(1, &mFaceIndicesBuffer);

    glGenVertexArrays(1, &mVAO);

    int mPositionLocation = 0;
    int mNormalLocation = 1;
    int mTextureCoordLocation = 2;

    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, getVertexBuffer());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getFaceIndicesBuffer());

    glEnableVertexAttribArray(mPositionLocation);
    glVertexAttribPointer(mPositionLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 3, (void*) 0);

    glBindBuffer(GL_ARRAY_BUFFER, getVertexNormalBuffer());

    glEnableVertexAttribArray(mNormalLocation);
    glVertexAttribPointer(mNormalLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 3, (void*) 0);

    glBindBuffer(GL_ARRAY_BUFFER, getTextureCoordinatesBuffer());
    glEnableVertexAttribArray(mTextureCoordLocation);
    glVertexAttribPointer(mTextureCoordLocation, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 2, (void*) 0);

    glBindVertexArray(0);

}

unsigned int ZMesh::getVAO() {
    return mVAO;
}

void ZMesh::setVertices(vector<float> vertices) {
	mVertices = std::move(vertices);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), &mVertices[0], GL_STATIC_DRAW);
}

void ZMesh::setVertexNormals(vector<float> normals) {
	mVertexNormals = std::move(normals);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVertexNormals.size() * sizeof(float), &mVertexNormals[0], GL_STATIC_DRAW);
}

void ZMesh::setTextureCoordinates(vector<float> coords) {
	mTextureCoordinates = std::move(coords);
    glBindBuffer(GL_ARRAY_BUFFER, mTextureCoordinatesBuffer);
    glBufferData(GL_ARRAY_BUFFER, mTextureCoordinates.size() * sizeof(float), &mTextureCoordinates[0], GL_STATIC_DRAW);
	
}

void ZMesh::setFaceIndices(vector<int> indices) {
	mFaceIndices = std::move(indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mFaceIndices.size() * sizeof(int), &mFaceIndices[0], GL_STATIC_DRAW);
}

vector<float> ZMesh::getVertices() const {
	return mVertices;
}

vector<float> ZMesh::getVertexNormals() const {
	return mVertexNormals;
}

vector<float> ZMesh::getTextureCoordinates() const {
	return mTextureCoordinates;
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

GLuint ZMesh::getTextureCoordinatesBuffer() {
	return mTextureCoordinatesBuffer;
}


GLuint ZMesh::getFaceIndicesBuffer() {
	return mFaceIndicesBuffer;
}

int ZMesh::getFaceIndiceCount() {
	return mFaceIndices.size();
}


