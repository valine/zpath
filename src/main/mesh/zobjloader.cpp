#include "zobjloader.h"

ZObjLoader::ZObjLoader(string resourcePath) {


}

ZMesh* ZObjLoader::loadMesh(string fileName) {

	vector<float> vertices;

	vertices.push_back(-1);
	vertices.push_back(-1);
	vertices.push_back(0.0);

	vertices.push_back(1.0);
	vertices.push_back(-1);
	vertices.push_back(0.0);

	vertices.push_back(-1);
	vertices.push_back(1.0);
	vertices.push_back(0.0);

	vertices.push_back(0);
	vertices.push_back(1);
	vertices.push_back(1);

	vector<int> faceIndices;

	faceIndices.push_back(0);
	faceIndices.push_back(1);
	faceIndices.push_back(2);
	faceIndices.push_back(1);
	faceIndices.push_back(2);
	faceIndices.push_back(3);

	ZMesh* mesh = new ZMesh();
	mesh->setVertices(vertices);
	mesh->setFaceIndices(faceIndices);

	return mesh;
}

ZObject* ZObjLoader::loadObject(string fileName) {

	ZMesh* mesh = loadMesh(fileName);

	ZObject* object = new ZObject();
	object->setMesh(mesh);

	return object;
}