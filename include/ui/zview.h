#ifndef ZVIEW_H
#define ZVIEW_H

#include <string> 
#include <iostream>
#include "glad/glad.h"

#include <GLFW/glfw3.h>
#include "ui/zshader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ui/ztexture.h"

#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H  

using namespace std;
using namespace glm;


class ZView {

	public:

        ZView(float maxWidth, float maxHeight, ZView *parent);

        enum Gravity {
		    topLeft,
		    topRight,
		    bottomLeft,
		    bottomRight
		};

		enum Bounds {

		};

		static const int fillParent = 100000;

		ZView(float maxWidth, float maxHeight);
		ZView(Bounds maxWidth, float maxHeight);
		ZView(float maxWidth, Bounds maxHeight);
		ZView(Bounds maxWidth, Bounds maxHeight);

        virtual void onWindowChange(int windowWidth, int windowHeight);
        virtual void onKeyPress(int key, int scancode, int action, int mods);
        virtual void onMouseEvent(int button, int action, int mods, int x, int y);
        virtual void onCursorPosChange(double x, double y);
        virtual void onScrollChange(double x, double y);
        virtual void onFileDrop(int count, const char** paths);


        // Lifecycle
        virtual void draw();
        void invalidate();
        bool needsRender();
        virtual void onCreate();
        virtual void onExit();

        // OpenGL shader
        virtual void setShader(ZShader *shader);
        virtual void setTextShader(ZShader *shader);
        virtual void setImageShader(ZShader *shader);

        ZShader* getTextShader();
        ZShader* getImageShader();

        // View hierarchy
        virtual void addSubView(ZView *view);
        void clearSubViews();
        void setParentView(ZView *parentView);
        ZView* getParentView();
        vector<ZView*> getSubViews();

        // Margin
        void setMargin(int marginLeft, int marginTop, int marginRight, int marginBottom);
        void setMargin(vec4 margin);
        int getMarginLeft();
        int getMarginTop();
        int getMarginRight();
        int getMarginBottom();

        // Offset
        void setOffset(double x, double y);
        void setYOffset(int y);
        void offsetBy(int x, int y);
        double getOffsetX();
        double getOffsetY();

        int getLeft();
        int getTop();
        int getRight();
        int getBottom();

        // View width / height
        virtual int getMaxWidth();
        virtual int getMaxHeight();

        int getWidth();
        int getHeight();

        void setWindowWidth(int width);
        void setWindowHeight(int height);

        int getWindowHeight();
        int getWindowWidth();

        void setMaxWidth(int width);
        void setMaxHeight(int height);

        virtual void computeBounds(int windowHeight, int maxWidth);

        // Misc view properties
        void setGravity(ZView::Gravity gravity);

        void setVisibility(bool visible);
        bool getVisibility();

        void setBackgroundColor(float color[4]);
        void setBackgroundColor(vec4 color);
        vec4 getBackgroundColor();
        void setBackgroundImage(ZTexture* background);

        void setTag(string);
        string getTag();

        // Mouse pointer / input state
		bool mouseIsDown();
		bool middleMouseIsDown();
		bool shiftKeyPressed();
		bool altKeyPressed();

		int getMouseDownX();
		int getMouseDownY();

		double getLastX();
		double getLastY();

	private:

		void init(int width, int height);

		bool mNeedsRender = true;

		Gravity mGravity;
		ZView *mParentView;

		ZShader *mShader = nullptr;
		ZShader *mTextShader;
		ZShader *mImageShader;
		float mMaxWidth = 0; 
		float mMaxHeight = 0;

		vec4 mBackgroundColor = vec4(0);

		GLuint mVertexBuffer = 0;
		GLuint mTexBuffer = 0;
		GLuint mFaceIndicesBuffer = 0;

		float mVertices[4*4] = {0,0,0,0,  0,0,1,0,  0,0,0,1, 0,0,1,1};
		float mTexCoords[2*4] = {-1,-1, -1,1, 1,-1, 1,1};

		int mFaceIndices[6] = {2,1,0,1,2,3};

		int mColorLocation;
		int mPositionLocation;
		int mTexCoordLocation;

		int mMarginLeft = 0;
		int mMarginTop = 0;
		int mMarginRight = 0;
		int mMarginBottom = 0;

		int left = 0;
		int right = 0; 
		int top = 0;
		int bottom = 0; 

		double mOffsetX = 0;
		double mOffsetY = 0;

		int mParentWidth = 100;
		int mParentHeight = 100;

		int mMouseX = 0;
		int mMouseY = 0;

		double mLastX;
		double mLastY;

		int mMouseDownX;
		int mMouseDownY;

		int mWindowWidth = 0;
		int mWindowHeight = 0;

		bool mMouseDown = false;
		bool mMiddleMouseDown = false;

		bool mShiftKeyPressed = false;
		bool mAltKeyPressed = false;

		bool mVisible = true;

		ZTexture* mBackgroundImage = nullptr;
		vector<ZView*> mViews;
		string mTag;
};

#endif