#ifndef ZMEHSUTILS_H
#define ZMEHSUTILS_H


#include "zmesh.h"
#include <iostream>
#include <vector> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

class ZMeshUtils {

public:

	ZMeshUtils();
	vec3 calculateBoundingBoxCenter(ZMesh* mesh);
	static void exportObj(string path, ZMesh* mesh);
private:

};

#endif