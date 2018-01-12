#include "zshader.h"
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


ZShader::ZShader(const char* vertexPath, const char* fragmentPath) {

 // 1. retrieve the vertex/fragment source code from filePath

  //       std::ifstream vertexFile(vertexPath);
		// std::string vertexCodeStr;
		// vertexFile.seekg(0, std::ios::end);   
		// vertexCodeStr.reserve(vertexFile.tellg());
		// vertexFile.seekg(0, std::ios::beg);
		// vertexCodeStr.assign((std::istreambuf_iterator<char>(vertexFile)),
  //           std::istreambuf_iterator<char>());
  // 		std::ifstream fragmentFile(fragmentPath);
		
		// std::string fragmentCodeStr;
		// fragmentFile.seekg(0, std::ios::end);   
		// fragmentCodeStr.reserve(fragmentFile.tellg());
		// fragmentFile.seekg(0, std::ios::beg);
		// fragmentCodeStr.assign((std::istreambuf_iterator<char>(fragmentFile)),
  //           std::istreambuf_iterator<char>());


		std::ifstream vT(vertexPath);
		std::stringstream vBuffer;
		vBuffer << vT.rdbuf();
		std::string vertexCodeStr = vBuffer.str();

		std::ifstream fT(fragmentPath);
		std::stringstream fBuffer;
		fBuffer << fT.rdbuf();
		std:string fragmentCodeStr = fBuffer.str();

		cout << fragmentCodeStr;


		const char* vertexCode = vertexCodeStr.c_str();
        const char * fragmentCode = fragmentCodeStr.c_str();

            // vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    // glCompileShader(vertex_shader);
    // fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    // glCompileShader(fragment_shader);
   
    // program = glCreateProgram();
   
    // glAttachShader(program, vertex_shader);
    // glAttachShader(program, fragment_shader);
    // glLinkProgram(program);


       
        // 2. compile shaders
        GLuint vertex, fragment;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        //fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        mID = glCreateProgram();
        glAttachShader(mID, vertex);
        glAttachShader(mID, fragment);
        glLinkProgram(mID);
        checkCompileErrors(mID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
      //  glDeleteShader(vertex);
       // glDeleteShader(fragment);

}

void ZShader::use() {

	glUseProgram(mID);
}

void ZShader::checkCompileErrors(unsigned int shader, std::string type) {

	 int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
}