#include "ui/zshader.h"

ZShader::ZShader(string vertexCodeStr, string fragmentCodeStr) {

    mVsName = vertexCodeStr;
    mFsName = fragmentCodeStr;
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

void ZShader::setBool(const std::string &name, bool value) const {         
    glUniform1i(glGetUniformLocation(mID, name.c_str()), (int)value); 
}

void ZShader::setInt(const std::string &name, int value) const { 
    glUniform1i(glGetUniformLocation(mID, name.c_str()), value); 
}

void ZShader::setFloat(const std::string &name, float value) const { 
    glUniform1f(glGetUniformLocation(mID, name.c_str()), value); 
}

void ZShader::setVec2(const std::string &name, const vec2 &value) const { 
    glUniform2fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]); 
}
void ZShader::setVec2(const std::string &name, float x, float y) const { 
    glUniform2f(glGetUniformLocation(mID, name.c_str()), x, y); 
}

void ZShader::setVec3(const std::string &name, vec3 value) const { 
    glUniform3fv(glGetUniformLocation(mID, name.c_str()), 1,  &value[0]); 
}

void ZShader::setVec3(const std::string &name, uint size, float* value) const { 
    glUniform3fv(glGetUniformLocation(mID, name.c_str()), size, value); 
}

void ZShader::setVec3(const std::string &name, float x, float y, float z) const { 
    glUniform3f(glGetUniformLocation(mID, name.c_str()), x, y, z); 
}

void ZShader::setVec4(const std::string &name, const vec4 &value) const { 
    glUniform4fv(glGetUniformLocation(mID, name.c_str()), 1, &value[0]); 
}

void ZShader::setVec4(const std::string &name, float x, float y, float z, float w)  { 
    glUniform4f(glGetUniformLocation(mID, name.c_str()), x, y, z, w); 
}

void ZShader::setMat2(const std::string &name, const mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ZShader::setMat3(const std::string &name, const mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ZShader::setMat4(const std::string &name, const mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(mID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
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
                cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << mVsName << " " << mFsName << " " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << mVsName << " " << mFsName << " "  << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
}