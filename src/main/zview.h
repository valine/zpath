#ifndef ZVIEW_H
#define ZVIEW_H

#include <string> 
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "zshader.h"

using std::string; 


class ZView {
	
	public:
		ZView(float maxWidth, float maxHeight, string dataPath, int shaderID);
		void draw();
		void setShader(int shaderID);
		void onWindowChange(int windowWidth, int windowHeight);
		void setMargin(int marginLeft, int marginTop, int marginRight, int marginBottom);
	private:
		int mShaderID;
		float mMaxWidth; 
		float mMaxHeight;

		float mBackgroundColor[4];

		GLuint mVertexBuffer;
		GLuint mFaceIndicesBuffer;

		float mVertices[3 * 4];
		int mFaceIndices[6];

		int mColorLocation;
		int mPositionLocation;

		int mMarginLeft;
		int mMarginTop;
		int mMarginRight;
		int mMarginBottom;

		int mParentWidth;
		int mParentHeight;

		void computeBounds(int windowHeight, int maxWidth);
};

#endif