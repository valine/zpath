#ifndef ZOBJECT_H
#define ZOBJECT_H
#include <vector>
#include "mesh/zmesh.h"
using std::vector;


class ZObject {
	
public:
	void setMesh(ZMesh mesh);
	ZMesh getMesh();

private: 
	ZMesh mMesh;
};

#endif