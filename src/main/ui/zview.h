#ifndef ZVIEW_H
#define ZVIEW_H

#include <string> 
#include <iostream>
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ui/zshader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H  

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

		void setWindowWidth(int width);
		void setWindowHeight(int height);

		int getWindowHeight();

		void setBackgroundColor(float color[4]);
		void setGravity(ZView::Gravity gravity);

		void setMaxWidth(int width);
		void setMaxHeight(int height);

		bool mouseIsDown();
		int getMouseDownX();
		int getMouseDownY();

		int getLastX();
		int getLastY();

		ZView* getParentView();


		virtual void draw();
		virtual void setShader(ZShader *shader);
		virtual void setTextShader(ZShader *shader);

		ZShader* getTextShader();

		virtual void computeBounds(int windowHeight, int maxWidth);

		virtual void onKeyPress(int key, int scancode, int action, int mods);
		virtual void onMouseEvent(int button, int action, int mods, int x, int y);
		virtual void onCursorPosChange(double x, double y);
		virtual void onScrollChange(double x, double y);

		vector<ZView*> getSubViews();
	private:

		void init(int width, int height);

		Gravity mGravity;
		ZView *mParentView;

		ZShader *mShader;
		ZShader *mTextShader;
		float mMaxWidth = 0; 
		float mMaxHeight = 0;

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

		int mParentWidth = 100;
		int mParentHeight = 100;

		int mMouseX = 0;
		int mMouseY = 0;

		int mLastX;
		int mLastY;

		int mMouseDownX;
		int mMouseDownY;

		int mWindowWidth = 0;
		int mWindowHeight = 0;

		bool mMouseDown = false;

		vector<ZView*> mViews;
};

#endif