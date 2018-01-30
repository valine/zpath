#include "zshader.h"

ZShader::ZShader(string vertexPathStr, string fragmentPathStr) {

		//cout << vertexPathStr;
		const char * vertexPath = vertexPathStr.c_str();
		const char * fragmentPath = fragmentPathStr.c_str();

		ifstream vT(vertexPath);
		stringstream vBuffer;
		vBuffer << vT.rdbuf();
		string vertexCodeStr = vBuffer.str();

		ifstream fT(fragmentPath);
		stringstream fBuffer;
		fBuffer << fT.rdbuf();
		string fragmentCodeStr = fBuffer.str();

		//cout << vertexCodeStr << "code";

		const char* vertexCode = vertexCodeStr.c_str();
        const char * fragmentCode = fragmentCodeStr.c_str();

        GLuint vertex, fragment;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
      
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        mID = glCreateProgram();
        glAttachShader(mID, vertex);
        glAttachShader(mID, fragment);
        glLinkProgram(mID);
        checkCompileErrors(mID, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);

}

void ZShader::use() {

	glUseProgram(mID);
}

void ZShader::checkCompileErrors(unsigned int shader, string type) {

	 int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
}