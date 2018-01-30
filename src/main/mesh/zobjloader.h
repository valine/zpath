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
	ZObjLoader(string resourcePath);

	ZMesh* loadMesh(string fileName);
	ZObject* loadObject(string fileName);
	
private:
	template<typename Out>
	void split(const string &s, char delim, Out result);

	vector<string> split(const string &s, char delim);

};

#endif