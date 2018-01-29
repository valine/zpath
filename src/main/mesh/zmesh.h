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
	void setFaceIndices(vector<int> indices);

	vector<float> getVertices() const; 
	vector<int> getFaceIndices() const; 

	GLuint getVertexBuffer();
	GLuint getFaceIndicesBuffer();

private:

	vector<float> mVertices;
	vector<int> mFaceIndices;

	GLuint mVertexBuffer;
	GLuint mFaceIndicesBuffer;
};

#endif