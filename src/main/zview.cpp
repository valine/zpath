

#include "zview.h"

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/string_cast.hpp"

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
using std::string;
using glm::mat4;


ZView::ZView(float maxWidth, float maxHeight, string mDataPath, int shaderID) {

	mMaxWidth = maxWidth;
	mMaxHeight = maxHeight;


    mVertices[0] = 0;
    mVertices[1] = 0;
    mVertices[2] = 0;

	mVertices[3] = mMaxWidth;
    mVertices[4] = 0;
    mVertices[5] = 0;

    mVertices[6] = 0;
    mVertices[7] = mMaxHeight;
	mVertices[8] = 0; 

    mVertices[9] = 0;
	mVertices[10] = mMaxWidth;
	mVertices[11] = mMaxHeight;


	mFaceIndices[0] = 0;
	mFaceIndices[1] = 1;
	mFaceIndices[2] = 2;

	mFaceIndices[3] = 1;
	mFaceIndices[4] = 2;
	mFaceIndices[5] = 3;


    GLint mvp_location, vpos_location, vcol_location;
    
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices, GL_STATIC_DRAW);
  
    // std::string vertexPath = mDataPath + "resources/shaders/uivertexshader.glsl";
    // std::string fragmentPath = mDataPath + "resources/shaders/uifragmentshader.glsl";
   
    // ZShader shader = ZShader(vertexPath, fragmentPath);
  
    mvp_location = glGetUniformLocation(shaderID, "MVP");
    vpos_location = glGetAttribLocation(shaderID, "vPos");
    vcol_location = glGetAttribLocation(shaderID, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 3, (void*) 0);


}

void ZView::draw() {


       //  mat4 matrix;


       // // glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
       //  matrix = glm::rotate(matrix, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
       //  matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, -3.0f));

        glDrawArrays(GL_TRIANGLES, 0, 6);
}

void ZView::setShader(int shader) {
	mShaderID = shader;
}