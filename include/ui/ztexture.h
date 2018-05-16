#ifndef ZTEXTURE_H
#define ZTEXTURE_H

#include "stb_image.h"
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

class ZTexture {

public:
	ZTexture(string path);
	unsigned int loadTexture(char const * path);
	uint getID();

private:
	uint mTextureID; 
};

#endif

