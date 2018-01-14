#ifndef ZSHADER_H
#define ZSHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using std::string;
using std::ifstream;
using std::cout;
using std::stringstream;

class ZShader {

public:
	GLuint mID;
	ZShader(string, string);
	void use();
private:
	void checkCompileErrors(GLuint shader, string type);
};

#endif