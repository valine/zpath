#include <string>
#define GLEW_STATIC
#include <GL/glew.h>

class ZShader {

public:
	GLuint mID;
	ZShader(std::string, std::string);
	void use();
private:
	void checkCompileErrors(GLuint shader, std::string type);
};