

#include "zview.h"

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/string_cast.hpp"

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using std::cout;
using std::endl;
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

    mVertices[9] = mMaxWidth;
	mVertices[10] = mMaxHeight;
	mVertices[11] = 0;

	mFaceIndices[0] = 0;
	mFaceIndices[1] = 1;
	mFaceIndices[2] = 2;

	mFaceIndices[3] = 1;
	mFaceIndices[4] = 2;
	mFaceIndices[5] = 3;

    mBackgroundColor[0] = 0;
    mBackgroundColor[1] = 0;
    mBackgroundColor[2] = 1.0;
    mBackgroundColor[3] = 1.0;


    mShaderID = shaderID;

    GLint vpos_location, vcol_location;
    
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mFaceIndicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mFaceIndices) * sizeof(int), mFaceIndices, GL_STATIC_DRAW);
  
    mPositionLocation = glGetAttribLocation(shaderID, "vPos");
    mColorLocation = glGetUniformLocation(shaderID, "uColor");

    glEnableVertexAttribArray(mPositionLocation);
    glVertexAttribPointer(mPositionLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 3, (void*) 0);
}

void ZView::onWindowChange(int windowWidth, int windowHeight) {
    mParentWidth = windowWidth;
    mParentHeight = windowHeight;

    computeBounds(windowWidth, windowHeight);
}

void ZView::setMargin(int marginLeft, int marginTop, int marginRight, int marginBottom) {

    mMarginLeft = marginLeft;
    mMarginTop = marginTop;
    mMarginRight = marginRight;
    mMarginBottom = marginBottom;

    computeBounds(mParentWidth, mParentWidth);
}

void ZView::computeBounds(int windowWidth, int windowHeight) {

    int left = 0 + mMarginLeft; // TODO: Surrounding view need to be accounted for
    int right; 
    int top = 0 + mMarginTop; // TODO: Surrounding view need to be accounted for
    int bottom = mMaxHeight - mMarginBottom; //TODO: bottom of window should be taken into acount

    if (windowWidth < mMaxWidth) {
        right = windowWidth - mMarginRight;
    } else {
        right = mMaxWidth - mMarginRight;
    }
    // if (windowWidth < mMaxWidth) {
    //     mVertices[3] = windowWidth + mMarginLeft;
    //     mVertices[9] = windowWidth;
    //     glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    //     glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices, GL_STATIC_DRAW);
    // } else {
    //     mVertices[3] = mMaxWidth;
    //     mVertices[9] = mMaxWidth;
    //     glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    //     glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices, GL_STATIC_DRAW);
    // }

    mVertices[0] = left;
    mVertices[1] = top;

    mVertices[3] = right;
    mVertices[4] = top;

    mVertices[6] = left;
    mVertices[7] = bottom;

    mVertices[9] = right;
    mVertices[10] = bottom;

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices, GL_STATIC_DRAW);

}



void ZView::draw() {

     glProgramUniform4f(mShaderID, mColorLocation,
        mBackgroundColor[0], mBackgroundColor[1], 
        mBackgroundColor[2], mBackgroundColor[3]);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); 

}


void ZView::setShader(int shader) {
	mShaderID = shader;
}