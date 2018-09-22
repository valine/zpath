#ifndef ZTEXTURE3D_H
#define ZTEXTURE3D_H

#include "glad/glad.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

class ZTexture3D {

public:
	ZTexture3D(string[]);
	unsigned int loadTexture(string[]);
	uint getID();

private:
	uint mTextureID; 
};

#endif

