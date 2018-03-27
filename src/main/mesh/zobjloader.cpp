#include "zobjloader.h"

ZObjLoader::ZObjLoader() {
}

ZMesh* ZObjLoader::loadMesh(string fileName) {
	
	ifstream infile(fileName);
	string line;

	cout<<fileName<<endl;
	
	vector<float> vertices;
	vector<int> faceIndices;
	vector<float> vertexNormals;
	vector<float> textureCoordinates;

	vector<float> tmpNormals;
	vector<float> tmpTexCoords;
	vector<float> texCoordsIndices;
	vector<float> vertexNormalIndices;

	while (getline(infile, line)) {

		string space = " ";
	    vector<string> tokens = split(line, space);

	   	if (tokens.at(0) == "v") {
	   		vertices.push_back(stod(tokens.at(1)));
	   		vertices.push_back(stod(tokens.at(2)));
	   		vertices.push_back(stod(tokens.at(3)));

	   		vertexNormals.push_back(stod(tokens.at(1)));
	   		vertexNormals.push_back(stod(tokens.at(2)));
	   		vertexNormals.push_back(stod(tokens.at(3)));

	   		textureCoordinates.push_back(stod(tokens.at(1)));
	   		textureCoordinates.push_back(stod(tokens.at(2)));
	   	}
	   	else if (tokens.at(0) == "vn") {
			tmpNormals.push_back(stod(tokens.at(1)));
	   		tmpNormals.push_back(stod(tokens.at(2)));
	   		tmpNormals.push_back(stod(tokens.at(3)));
	   	}
	   	else if (tokens.at(0) == "vt") {
			tmpTexCoords.push_back(stod(tokens.at(1)));
	   		tmpTexCoords.push_back(stod(tokens.at(2)));
	   	}

	   	else if (tokens.at(0) == "f") {
	   		string delim = "/";
	   		vector<string> indiceTokensA = split(tokens.at(1), delim);
	   		vector<string> indiceTokensB = split(tokens.at(2), delim);
	   		vector<string> indiceTokensC = split(tokens.at(3), delim);

	   		faceIndices.push_back(stoi(indiceTokensA.at(0)) - 1);
	   		faceIndices.push_back(stoi(indiceTokensB.at(0)) - 1);
	   		faceIndices.push_back(stoi(indiceTokensC.at(0)) - 1);

	   		vertexNormalIndices.push_back(stoi(indiceTokensA.at(2)) - 1);
	   		vertexNormalIndices.push_back(stoi(indiceTokensB.at(2)) - 1);
	   		vertexNormalIndices.push_back(stoi(indiceTokensC.at(2)) - 1);

	   		if (indiceTokensA.at(1) != "") {
		   		texCoordsIndices.push_back(stoi(indiceTokensA.at(1)) - 1);
		   		texCoordsIndices.push_back(stoi(indiceTokensB.at(1)) - 1);
	   			texCoordsIndices.push_back(stoi(indiceTokensC.at(1)) - 1);
	   		}
	   	}
	}

	for (unsigned i = 0; i < vertexNormalIndices.size(); ++i) {

		int vertex = faceIndices[i];

		vertexNormals.at(vertex * 3 + 0) = tmpNormals.at(vertexNormalIndices[i] * 3 + 0);
	   	vertexNormals.at(vertex * 3 + 1) = tmpNormals.at(vertexNormalIndices[i] * 3 + 1);
	   	vertexNormals.at(vertex * 3 + 2) = tmpNormals.at(vertexNormalIndices[i] * 3 + 2);
	}

	for (unsigned i = 0; i < texCoordsIndices.size(); ++i) {
		int vertex = faceIndices[i];
		textureCoordinates.at(vertex * 2 + 0) = tmpTexCoords.at(texCoordsIndices[i] * 2 + 0);
	   	textureCoordinates.at(vertex * 2 + 1) = tmpTexCoords.at(texCoordsIndices[i] * 2 + 1);
	}

	ZMesh* mesh = new ZMesh();
	mesh->setVertices(vertices);
	mesh->setFaceIndices(faceIndices);
	mesh->setVertexNormals(vertexNormals);
	mesh->setTextureCoordinates(textureCoordinates);

	return mesh;
}

vector<string> ZObjLoader::split(string str, string delims) {
	std::vector<std::string> cont;
    std::size_t current, previous = 0;
    current = str.find_first_of(delims);
    while (current != std::string::npos) {
        cont.push_back(str.substr(previous, current - previous));
        previous = current + 1;
        current = str.find_first_of(delims, previous);
    }
    cont.push_back(str.substr(previous, current - previous));
    return cont;

}

ZObject* ZObjLoader::loadObject(string fileName) {

	ZMesh* mesh = loadMesh(fileName);

	ZObject* object = new ZObject();
	object->setMesh(mesh);

	return object;
}