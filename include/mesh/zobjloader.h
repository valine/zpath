#ifndef ZOBJLOADER_H
#define ZOBJLOADER_H

#include <string>
#include "mesh/zmesh.h"
#include "mesh/zobject.h"
#include "mesh/zmeshutils.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <algorithm>
#include <iterator>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std; 

class ZObjLoader {
	
public:
	ZObjLoader();
	vector<ZObject*> loadObjects(const std::string& pFile);
	ZMesh* loadMesh(string fileName);
	ZObject* loadObject(string fileName);
	
private:
	vector<string> split(string s, string delim);
	vector<ZObject*> processNode(aiNode *node, const aiScene *scene);
	ZMesh* convertAiMesh(aiMesh* mesh);
};

#endif