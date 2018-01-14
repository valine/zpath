#ifndef ZSHADER_H
#define ZSHADER_H

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>

class ZShader {

public:
	GLuint mID;
	ZShader(std::string, std::string);
	ZShader();
	void use();
private:
	void checkCompileErrors(GLuint shader, std::string type);
};

#endif