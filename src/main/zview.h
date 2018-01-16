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
		enum Gravity {
		    topLeft,
		    topRight,
		    bottomLeft,
		    bottomRight
		};

		enum Bounds {
		    fillParent
		};

		ZView(float maxWidth, float maxHeight);
		ZView(Bounds maxWidth, float maxHeight);
		ZView(float maxWidth, Bounds maxHeight);
		ZView(Bounds maxWidth, Bounds maxHeight);

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

		int getMaxWidth();
		int getMaxHeight();

		int getWidth();
		int getHeight();

		void setBackgroundColor(float color[4]);
		void setGravity(ZView::Gravity gravity);

		void setMaxWidth(int width);
		void setMaxHeight(int height);

		bool mouseIsDown();
		int getMouseDownX();
		int getMouseDownY();

		virtual void computeBounds(int windowHeight, int maxWidth);

		virtual void onKeyPress(int key, int scancode, int action, int mods);
		virtual void onMouseEvent(int button, int action, int mods);
		virtual void onCursorPosChange(double x, double y);

		vector<ZView*> getSubViews();
	private:

		void init(int width, int height);

		Gravity mGravity;
		ZView *mParentView;

		ZShader *mShader;
		float mMaxWidth; 
		float mMaxHeight;

		float mBackgroundColor[4] = {0,0,0,0};

		GLuint mVertexBuffer;
		GLuint mFaceIndicesBuffer;

		float mVertices[3 * 4] = {0};
		int mFaceIndices[6] = {0,1,2,1,2,3};

		int mColorLocation;
		int mPositionLocation;

		int mMarginLeft = 0;
		int mMarginTop = 0;
		int mMarginRight = 0;
		int mMarginBottom = 0;

		int left = 0;
		int right = 0; 
		int top = 0;
		int bottom = 0; 

		int mOffsetX = 0;
		int mOffsetY = 0;

		int mParentWidth;
		int mParentHeight;

		int mMouseX;
		int mMouseY;

		int mMouseDownX;
		int mMouseDownY;

		bool mMouseDown;

		vector<ZView*> mViews;
};

#endif