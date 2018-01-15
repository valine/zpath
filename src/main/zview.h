#ifndef ZVIEW_H
#define ZVIEW_H

#include <string> 
#include <iostream>
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "zshader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

using std::string; 
using std::cout;
using std::endl;
using glm::mat4;
using std::vector;

class ZView {
	
	public:
		ZView(float maxWidth, float maxHeight);

		enum Gravity {
		    topLeft,
		    topRight,
		    bottomLeft,
		    bottomRight
		};

		void draw();
		void setShader(ZShader *shader);
		void onWindowChange(int windowWidth, int windowHeight);
		void setMargin(int marginLeft, int marginTop, int marginRight, int marginBottom);
		void setOffset(int x, int y);

		void addSubView(ZView *view);

		void setParentView(ZView *parentView);

		int getOffsetX();
		int getOffsetY();

		int getMarginLeft();
		int getMarginTop();
		int getMarginRight();
		int getMarginBottom();

		int getLeft();
		int getTop();
		int getRight();
		int getBottom();

		int getWidth();
		int getHeight();

		void setBackgroundColor(float color[4]);
		void setGravity(ZView::Gravity gravity);

		vector<ZView*> getSubViews();
	private:

		Gravity mGravity;
		ZView *mParentView;

		ZShader *mShader;
		float mMaxWidth; 
		float mMaxHeight;

		float mBackgroundColor[4] = {0.1};

		GLuint mVertexBuffer;
		GLuint mFaceIndicesBuffer;

		float mVertices[3 * 4] = {0};
		int mFaceIndices[6] = {0,1,2,1,2,3};

		int mColorLocation;
		int mPositionLocation;

		int mMarginLeft;
		int mMarginTop;
		int mMarginRight;
		int mMarginBottom;

		int left = 0;
		int right = 0; 
		int top = 0;
		int bottom = 0; 

		int mOffsetX = 0;
		int mOffsetY = 0;

		int mParentWidth;
		int mParentHeight;

		vector<ZView*> mViews;


		void computeBounds(int windowHeight, int maxWidth);
};

#endif