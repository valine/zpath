#include "zobjloader.h"

ZObjLoader::ZObjLoader(string resourcePath) {


}

ZMesh* ZObjLoader::loadMesh(string fileName) {
	
	ifstream infile(fileName);
	string line;

	cout<<fileName<<endl;
	
	vector<float> vertices;
	vector<int> faceIndices;

	while (getline(infile, line)) {
	

	    vector<string> tokens = split(line, ' ');
	 

	   	if (tokens.at(0) == "v") {
	   		vertices.push_back(stod(tokens.at(1)));
	   		vertices.push_back(stod(tokens.at(2)));
	   		vertices.push_back(stod(tokens.at(3)));
	   	}

	   	else if (tokens.at(0) == "vn") {


	   	}

	   	else if (tokens.at(0) == "f") {
	   		vector<string> indiceTokensA = split(tokens.at(1), '//');
	   		vector<string> indiceTokensB = split(tokens.at(2), '//');
	   		vector<string> indiceTokensC = split(tokens.at(3), '//');

	   		faceIndices.push_back(stoi(indiceTokensA.at(0)) - 1);
	   		faceIndices.push_back(stoi(indiceTokensB.at(0)) - 1);
	   		faceIndices.push_back(stoi(indiceTokensC.at(0)) - 1);

	   	}
	}

	ZMesh* mesh = new ZMesh();
	mesh->setVertices(vertices);
	mesh->setFaceIndices(faceIndices);

	return mesh;
}

template<typename Out>
void ZObjLoader::split(const string &s, char delim, Out result) {
    stringstream ss(s);
    string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

vector<string> ZObjLoader::split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, back_inserter(elems));
    return elems;
}


ZObject* ZObjLoader::loadObject(string fileName) {

	ZMesh* mesh = loadMesh(fileName);

	ZObject* object = new ZObject();
	object->setMesh(mesh);

	return object;
}