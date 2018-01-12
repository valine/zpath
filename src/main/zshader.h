#include <string>
#define GLEW_STATIC
#include <GL/glew.h>

class ZShader {

public:
	GLuint mID;
	ZShader(const char* vertexPath, const char* fragmentPath);
	void use();
private:
	void checkCompileErrors(GLuint shader, std::string type);
};