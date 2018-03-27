#ifndef ZOBJLOADER_H
#define ZOBJLOADER_H

#include <string>
#include "mesh/zmesh.h"
#include "mesh/zobject.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>


#include <algorithm>
#include <iterator>
using namespace std; 

class ZObjLoader {
	
public:
	ZObjLoader();

	ZMesh* loadMesh(string fileName);
	ZObject* loadObject(string fileName);
	
private:
	//template<typename Out>

	vector<string> split(string s, string delim);
};

#endif