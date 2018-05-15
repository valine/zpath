#ifndef ZMESH_H
#define ZMESH_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector> 

using std::vector;

class ZMesh {
	
public:
	void setVertices(vector<float> vertices);
	void setVertexNormals(vector<float> normals);
	void setFaceIndices(vector<int> indices);
	void setTextureCoordinates(vector<float> coords);

	vector<float> getVertices() const; 
	vector<float> getVertexNormals() const;
	vector<float> getTextureCoordinates() const;
	vector<int> getFaceIndices() const; 

	int getFaceIndiceCount();

	GLuint getVertexBuffer();
	GLuint getVertexNormalBuffer();
	GLuint getFaceIndicesBuffer();
	GLuint getTextureCoordinatesBuffer();

private:
	vector<float> mVertices;
	vector<float> mVertexNormals;
	vector<int> mFaceIndices;
	vector<float> mTextureCoordinates;

	GLuint mVertexBuffer;
	GLuint mVertexNormalBuffer;
	GLuint mFaceIndicesBuffer;
	GLuint mTextureCoordinatesBuffer;
};

#endif