#ifndef ZMESH_H
#define ZMESH_H

#include <vector> 
using std::vector;

class ZMesh {
	
public:
	void setVertices(vector<float> vertices);
	void setFaceIndices(vector<int> indices);

	vector<float> getVertices() const; 
	vector<int> getFaceIndices() const; 

private:

	vector<float> mVertices;
	vector<int> mFaceIndices;
};

#endif