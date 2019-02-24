#ifndef ZTEXTURE_H
#define ZTEXTURE_H

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
	ZTexture(float* pixels, int width, int height);
	unsigned int loadTexture(char const * path);
	unsigned int loadTexture(float* pixels, int width, int height);
	uint getID();
	void updateTexture(float* pixels, int width, int height);

private:
	uint mTextureID; 
};

#endif

