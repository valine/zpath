#ifndef ZTEXTURE_H
#define ZTEXTURE_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

using namespace std;


class ZTexture {

public:
	unsigned int loadTexture(char const * path);

private:

};

#endif