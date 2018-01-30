#ifndef ZOBJLOADER_H
#define ZOBJLOADER_H

#include <string>
#include "mesh/zmesh.h"
#include "mesh/zobject.h"
#include <vector>

using std::vector;
using std::string;

class ZObjLoader {
	
public:
	ZObjLoader(string resourcePath);

	ZMesh* loadMesh(string fileName);
	ZObject* loadObject(string fileName);
	
private:



};

#endif