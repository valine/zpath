#include "zmesh.h"

void ZMesh::setVertices(vector<float> vertices) {
	mVertices = vertices;
}

void ZMesh::setFaceIndices(vector<int> indices) {
	mFaceIndices = indices;
}

vector<float> ZMesh::getVertices() const {
	return mVertices;
}

vector<int> ZMesh::getFaceIndices() const {
	return mFaceIndices;
}

