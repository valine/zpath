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
#include <utils/zshadowrenderer.h>
#include "utils/zsettingsstore.h"
#include FT_FREETYPE_H

using namespace std;
using namespace glm;


class ZView {

	public:

    ZView(float maxWidth, float maxHeight, ZView *parent);

    ZView(float maxWidth, float maxHeight, ZView *parent, bool isScrollable);

    ZView(float maxWidth, float maxHeight, vec4 bg, ZView *parent);

    ZView(vec2 size, ZView *parent);

    ZView(vec2 size, vec4 bg, ZView *parent);

    enum Gravity {
		    topLeft,
		    topRight,
		    bottomLeft,
		    bottomRight
		};

        enum WireType {
            full,
            outline,
            none
        };

		enum Bounds {

		};

        static const int mouseDown = 0;
        static const int mouseDrag = 1;
        static const int mouseUp = 2;
		static const int fillParent = 100000;

		ZView* mShadowView = nullptr;

		ZView(float maxWidth, float maxHeight);

        // Lifecycle
        virtual void draw();
        void invalidate();
        bool needsRender();
        virtual void onLayoutFinished();
        virtual void onCreate();
        virtual void onExit();

        void onDpChange(float dp);
        virtual void onWindowChange(int windowWidth, int windowHeight);
        virtual void onMouseEvent(int button, int action, int mods, int sx, int sy);
        virtual void onCursorPosChange(double x, double y);
        virtual void onScrollChange(double x, double y);
        virtual void onFileDrop(int count, const char** paths);
        virtual void onKeyPress(int key, int scancode, int action, int mods);
        virtual void onFocusChanged(ZView *viewWithFocus);


    virtual void onCharacterInput(unsigned int character);
        /**
         * Called when mouse up anywhere on the screen
         */
        virtual void onGlobalMouseUp(int key);

        virtual void onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button);
        virtual void onSizeChange();



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
        ZView* getRootView();
        void setRootView(ZView* rootView);
        void incrementZCursor();
        float getZPosition();
        void setZPosition(float z);
        float getZCursorPosition();

        virtual bool isViewInFocus();
        virtual void requestFocus(ZView* view);
        virtual void releaseFocus(ZView *forView);

    // Margin
        void setMargin(int marginLeft, int marginTop, int marginRight, int marginBottom);
        void setMargin(vec4 margin);
        int getMarginLeft();
        int getMarginTop();
        int getMarginRight();
        int getMarginBottom();

        void setMarginLeft(float m);
        void setMarginTop(float m);
        void setMarginRight(float m);
        void setMarginBottom(float m);
        vec4 getMargin();
        vec2 getCenter();

        // Offset
        void setOffset(double x, double y);
        void setYOffset(int y);
        void setXOffset(int x);
        void setOffset(vec2 pos);
        void offsetBy(int x, int y);
        double getOffsetX();
        double getOffsetY();
        vec2 getOffset();

        void setInitialPosition(vec2 position);
        void resetInitialPosition();
        vec2 getInitialPosition();

        int getLeft();
        int getTop();
        int getRight();
        int getBottom();

        int calculateLeft();
        int calculateTop();
        int calculateRight();
        int calculateBottom();

        // View width / height
        virtual int getMaxWidth();
        virtual int getMaxHeight();

        vec2 getSize();

        int getWidth();
        int getHeight();

        vec2 getScale();
        void setScale(vec2 scale);

        vec2 getInnerTranslation();
        void setInnerTranslation(vec2 scale);

        vec2 getRelativeScale();

        void setWindowWidth(int width);
        void setWindowHeight(int height);

        int getWindowHeight();
        int getWindowWidth();

        virtual void setMaxWidth(int width);
        virtual void setMaxHeight(int height);

        virtual void onMouseOver();
        virtual void onMouseLeave();

        virtual void computeBounds();

        // Misc view properties
        virtual void setGravity(ZView::Gravity gravity);
        Gravity getGravity();

        void setVisibility(bool visible);
        bool getVisibility();

        virtual void setBackgroundColor(float color[4]);
        virtual void setBackgroundColor(vec4 color);

        virtual void setHighlighColor(vec4 color) {
            mHighlightColor = color;
        }

        virtual void showHighlight(bool shouldShow) {
            mShowHighlight = shouldShow;
        }

        virtual vec4 getBackgroundColor();
        void setBackgroundImage(ZTexture* background);
        void setOutlineColor(vec4 color);

        void setTag(string);
        string getTag();

        // Mouse pointer / input state
        bool rightMouseIsDown();
		bool mouseIsDown();
		bool middleMouseIsDown();
		bool shiftKeyPressed();
		bool altKeyPressed();
		bool anyMouseDown();

		void setMouseDown(bool isDown);
        void setMiddleMouseDown(bool mouseDown);
        void setRightMouseDown(bool mouseDown);
		int getMouseDownX();
		int getMouseDownY();

		vec2 getMouseDragDelta();

		double getLastX();
		double getLastY();

		void setOutlineType(WireType wire);

		float* getVertices();

        GLuint getVertexBuffer();

        int getIndexTag();
        void setIndexTag(int index);

        void setClickable(bool);
        bool isClickable();

        void removeView();

        vec2 getMouse();

        vec2 getRelativeMouse();

        bool isMouseInBounds(ZView *view) const;

        void setLineWidth(float width);

        /**
         * Color library. Useful for quickly setting a color to a view for testing
         *
         */
        const vec4 white = vec4(1,1,1,1);
        const vec4 black = vec4(0,0,0,1);
        const vec4 red = vec4(1,0,0,1);
        const vec4 blue = vec4(0,0,1,1);
        const vec4 green = vec4(0,1,0,1);
        const vec4 yellow = vec4(1,1,0,1);
        const vec4 gold = vec4(1, 0.709230, 0, 1);
        const vec4 grey = vec4(0.7, 0.7, 0.7, 1.0);
        const vec4 darkGrey = vec4(0.5, 0.5, 0.5, 1.0);
        const vec4 darkerGrey = vec4(0.04, 0.04, 0.05, 1.0);
        const vec4 transparent = vec4(0);
        const vec4 faded = vec4(0.5, 0.5, 0.5, 0.2);

        const vec4 bg = ZSettingsStore::get().getBackgroundColor();
        const vec4 base =  ZSettingsStore::get().getBaseColor();
        const vec4 highlight =  ZSettingsStore::get().getHighlightColor();

        void setName(string name);
        string getName();
        void init();

        void setAllowNegativeSize(bool allow);

        void setElevation(float elevation) {
            mElevation = elevation;

        }

        virtual ~ZView();

        float getElevation();
        void setClippingEnabled(bool clipping) {
            mClipping = clipping;
        }

    bool mIsScrollable = true;

    void removeSubView(int index);

    void removeSubView(ZView *view);

    void bringToFront();

    void setTranslation(vec2 translation);

    vec2 getTranslation();

    void translateBy(vec2 translation);

    void setConsumeClicks(bool consume);

    float mDP = 1.0;

    int getLocalTop();
    int getLocalBottom();
    int getLocalLeft();
    int getLocalRight();

private:

    bool mNeedsRender = true;
		bool mVertsInvalid = false;

		bool mCreateComplete = false;

		Gravity mGravity = topLeft;
		ZView *mParentView{};
		ZView *mRootView{};

		/**
		 * The z cursor position gets incremented on the root view every
		 * time a view is added to the hierarchy
		 */
		float mZCursorPosition = 0;
		float mZPosition = 0;

		ZShader *mShader = nullptr;
		ZShader *mTextShader{};
		ZShader *mImageShader{};
		float mMaxWidth = 0; 
		float mMaxHeight = 0;

		vec4 mBackgroundColor = vec4(0);
        vec4 mHighlightColor = vec4(0);
		vec4 mOutlineColor = red;
		bool mShowHighlight = false;

		GLuint mVAO = 0;
        GLuint mEdgeVAO = 0;
        GLuint mOutlineVAO = 0;

        GLuint mVertexBuffer = 0;
		GLuint mTexBuffer = 0;
		GLuint mFaceIndicesBuffer = 0;
        GLuint mEdgeIndicesBuffer = 0;
        GLuint mOutlineIndicesBuffer = 0;

        static const int OUTLINE_INDEX_COUNT = 8;
        static const int EDGE_INDEX_COUNT = 10;
        static const int FACE_INDEX_COUNT = 6;

        float mVertices[4*4] = {0,0,0,0,  0,0,1,0,  0,0,0,1, 0,0,1,1};
		float mTexCoords[2*4] = {-1,-1, -1,1, 1,-1, 1,1};

		int mFaceIndices[6] = {2,1,0,1,2,3};
        int mEdgeIndices[10] = {2,1,1,0,0,1,1,2,2,3};

        int mOutlineIndices[10] = {3,1, 1,0, 0,2, 2,3};

		int mColorLocation{};
		int mPositionLocation{};
		int mTexCoordLocation{};

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
        // Elevation determines the size of the shadow
        float mElevation = 0.0;

        vec2 mScale = vec2(1);
        vec2 mScaleCache = mScale;
		vec2 mInnerTranslation = vec2(0);

		int mParentWidth = 100;
		int mParentHeight = 100;

		int mMouseX = 0;
		int mMouseY = 0;

		double mLastX{};
		double mLastY{};

		vec2 mMouseDragDelta{};

		int mMouseDownX{};
		int mMouseDownY{};

		int mWindowWidth = 0;
		int mWindowHeight = 0;

		bool mMouseDown = false;
		bool mMiddleMouseDown = false;
		bool mRightMouseDown = false;

		bool mShiftKeyPressed = false;
		bool mAltKeyPressed = false;

		bool mDeleted = false;
		bool mVisible = true;
		bool mClickable = true;

		bool mAllowNegativeSize = false;
		ZTexture* mBackgroundImage = nullptr;
		vector<ZView*> mViews;
		vector<ZView*> mPendingViews;
		string mTag;
		int mIndexTag{};

        int mLeft = 0;
        int mRight = 0;
        int mTop = 0;
        int mBottom = 0;

        WireType mDrawWire = none;
        vec2 mInitialPosition{};
        string mName;

        float mLineWidth = 2.0;

        bool mClipping = true;
        vec2 mTranslation = vec2(0);

        bool mMouseOver = false;

        bool mConsumeClicks = false;

    void drawShadow();

    void setup(float maxWidth, float maxHeight, ZView *parent);

    void cacheScale();

    void calculateBounds();

    void addViewFromDraw(ZView *view);
void setComsumeClicks();

    void clearFocus();
};

#endif