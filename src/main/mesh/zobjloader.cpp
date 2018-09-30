#include "mesh/zobjloader.h"

ZObjLoader::ZObjLoader() {
}

vector<ZObject*> ZObjLoader::loadObjects(const std::string& pFile) {

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile( pFile, 
	aiProcess_CalcTangentSpace       | 
	aiProcess_Triangulate            |
	aiProcess_JoinIdenticalVertices  |
	aiProcess_SortByPType);
	vector<ZObject*> objects;

	if(!scene) {
	    cout << importer.GetErrorString() << endl;
	    return objects;
	}

	aiNode* node = scene->mRootNode;
    return processNode(node, scene, nullptr);
}


vector<ZObject*> ZObjLoader::processNode(aiNode *node, const aiScene *scene, ZObject* parent) {
	vector<ZObject*> objects;

    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {

        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ZObject* object = new ZObject();

        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		aiColor3D color;
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);

		float alpha;
		mat->Get(AI_MATKEY_OPACITY, alpha);
		aiColor3D otherProperties;
		mat->Get(AI_MATKEY_COLOR_SPECULAR, otherProperties);

		ZMaterial* zmaterial = new ZMaterial(vec4(color.r,color.g,color.b, otherProperties.b));
		zmaterial->setRoughness(otherProperties.r);
		zmaterial->setMetallic(otherProperties.g);
		cout << endl << "Loading material" << endl <<
		"red:" << color.r << endl <<
		"green:" << color.g << endl << 
		"blue:" << color.b << endl <<
		"alpha:" << otherProperties.b << endl <<
		"roughness:" << otherProperties.r << endl <<
		"metallic:" << otherProperties.g << endl;
		
        ZMesh* convertedMesh = convertAiMesh(mesh);
        object->setMesh(convertedMesh);
       
       	cout << "before bounding box" << endl;
        ZMeshUtils meshutils = ZMeshUtils();
		object->setOrigin(meshutils.calculateBoundingBoxCenter(convertedMesh));

		cout << "after bounding box" << endl;
		object->setMaterial(zmaterial);
        objects.push_back(object);
    }

    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
    	//if (!objects.empty()) {
    		//ZObject* object = objects.back();
    	if (node->mNumMeshes > 0) {
	    	vector<ZObject*> children = processNode(node->mChildren[i], scene, objects.at(0));
	    	if (parent != nullptr) {
	        
	     
				for (vector<ZObject*>::iterator it = children.begin() ; it != children.end(); ++it) {
					ZObject *child = (*it);
					if (child != nullptr) {
						child->setParent(parent);
					}
				}
			}
			objects.insert(objects.end(), children.begin(), children.end());
		} else {
			vector<ZObject*> children = processNode(node->mChildren[i], scene, nullptr);
			objects.insert(objects.end(), children.begin(), children.end());
		}	
    }

    return objects;
}  

ZMesh* ZObjLoader::convertAiMesh(aiMesh* mesh) {
	vector<float> vertices;
	vector<int> faceIndices;
	vector<float> vertexNormals;
	vector<float> textureCoordinates;

	ZMesh* outputMesh = new ZMesh();

	cout << "vertices" << endl;
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
    	vertices.push_back(mesh->mVertices[i].x);
    	vertices.push_back(mesh->mVertices[i].y);
    	vertices.push_back(mesh->mVertices[i].z);

    	vertexNormals.push_back(mesh->mNormals[i].x);
    	vertexNormals.push_back(mesh->mNormals[i].y);
    	vertexNormals.push_back(mesh->mNormals[i].z);

    	if(mesh->mTextureCoords[0]) {

			textureCoordinates.push_back(mesh->mTextureCoords[0][i].x); 
			textureCoordinates.push_back(mesh->mTextureCoords[0][i].y); 
    	} else {
    		textureCoordinates.push_back(0.0); 
			textureCoordinates.push_back(0.0); 
    	}
    }

    cout << "faces" << endl;
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for(unsigned int j = 0; j < face.mNumIndices; j++) {
			faceIndices.push_back(face.mIndices[j]);
		}
    }
	cout << "after faces" << endl;
	outputMesh->setVertices(vertices);
	outputMesh->setFaceIndices(faceIndices);
	outputMesh->setVertexNormals(vertexNormals);
	outputMesh->setTextureCoordinates(textureCoordinates);
	cout << "after create output mesh" << endl;
	return outputMesh;
}

ZMesh* ZObjLoader::loadMesh(string fileName) {
	ifstream infile(fileName);

	if (!infile.good()) {
		return nullptr;
	}

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

	   		if (indiceTokensA.at(1) != "") {
		   		texCoordsIndices.push_back(stoi(indiceTokensA.at(1)) - 1);
		   		texCoordsIndices.push_back(stoi(indiceTokensB.at(1)) - 1);
	   			texCoordsIndices.push_back(stoi(indiceTokensC.at(1)) - 1);
	   		}

	   		vertexNormalIndices.push_back(stoi(indiceTokensA.at(2)) - 1);
	   		vertexNormalIndices.push_back(stoi(indiceTokensB.at(2)) - 1);
	   		vertexNormalIndices.push_back(stoi(indiceTokensC.at(2)) - 1);

	   	}
	}


	// vector<vector<int>> sharedVertexCount;

	// for (unsigned i = 0; i < vertices.size(); ++i) {
	// 	vector<int> v;
	// 	sharedVertexCount.push_back(v);
	// }


	// // Check for texture coordinate seams
	// for (unsigned i = 0; i < texCoordsIndices.size(); ++i) {
	// 	sharedVertexCount.at(faceIndices.at(i)).push_back(i);
	// }

	// for (unsigned i = 0; i < texCoordsIndices.size(); ++i) {
	// 	if (sharedVertexCount.at(faceIndices.at(i)).size() > 1) {
			
	// 		for (unsigned j = 0; j < sharedVertexCount.at(faceIndices.at(i)).size(); ++j) {
				
	// 			int index = sharedVertexCount.at(faceIndices.at(i)).at(j);


	// 			cout<<sharedVertexCount.at(faceIndices.at(i)).at(j)<<endl;
	// 		}

	// 		cout<<endl;
			
	// 	}
	// }

	for (unsigned i = 0; i < vertexNormalIndices.size(); ++i) {
		int vertex = faceIndices[i];
		vertexNormals.at(vertex * 3 + 0) = tmpNormals.at(vertexNormalIndices[i] * 3 + 0);
	   	vertexNormals.at(vertex * 3 + 1) = tmpNormals.at(vertexNormalIndices[i] * 3 + 1);
	   	vertexNormals.at(vertex * 3 + 2) = tmpNormals.at(vertexNormalIndices[i] * 3 + 2);
	}

	for (unsigned i = 0; i < texCoordsIndices.size(); ++i) {
		int vertex = faceIndices[i];
		textureCoordinates.at(vertex * 2 + 0) = tmpTexCoords.at(texCoordsIndices[i] * 2 + 0);
	   	textureCoordinates.at(vertex * 2 + 1) = -tmpTexCoords.at(texCoordsIndices[i] * 2 + 1);
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
	ZMeshUtils meshutils = ZMeshUtils();
	ZObject* object = new ZObject();
	object->setMesh(mesh);
	object->setOrigin(meshutils.calculateBoundingBoxCenter(mesh));

	return object;
}