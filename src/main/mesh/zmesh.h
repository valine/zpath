#ifndef ZMESH_H
#define ZMESH_H
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector> 


using std::vector;

class ZMesh {
	
public:
	void setVertices(vector<float> vertices);
	void setVertexNormals(vector<float> normals);
	void setFaceIndices(vector<int> indices);

	vector<float> getVertices() const; 
	vector<float> getVertexNormals() const;
	vector<int> getFaceIndices() const; 

	int getFaceIndiceCount();

	GLuint getVertexBuffer();
	GLuint getVertexNormalBuffer();
	GLuint getFaceIndicesBuffer();

private:

	vector<float> mVertices;
	vector<float> mVertexNormals;
	vector<int> mFaceIndices;

	GLuint mVertexBuffer;
	GLuint mVertexNormalBuffer;
	GLuint mFaceIndicesBuffer;
};

#endif