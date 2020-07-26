#ifndef ZVIEWCONTROLLER_H
#define ZVIEWCONTROLLER_H


#include <vector>
#include <string>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ui/zview.h"
#include "ui/zshader.h"
#include "ui/ztiledview.h"
#include "ui/z3dview.h"
#include "ui/zbutton.h"

#include "mesh/zscene.h"
#include "scenes/basicscene.h"
#include "mesh/zrenderer.h"

#include "zlabel.h"
#include "zonclicklistener.h"

#include "zslider.h"
#include "zsliderlistener.h"

#include "zcheckbox.h"
#include "zcheckboxlistener.h"


using glm::mat4;
using glm::ortho;
using std::vector;
using std::string;

class ZViewController : public ZView {

public:
	ZViewController(string resourcePath);
    ZViewController(char* argv[]);
    ZViewController(const char* argv[]);

    string getResourcePath();

    // Life cycle events driven by application
	virtual void onCreate();
    virtual void draw();
    virtual void onTerminate();

	// Glfw window events
    virtual void onWindowChange(int width, int height);
    virtual void onKeyPress(int key, int scancode, int action, int mods);
    virtual void onMouseEvent(int button, int action, int mods, int x, int y);
    virtual void onCursorPosChange(double x, double y);
    virtual void onScrollChange(double x, double y);
	virtual void onFileDrop(int count, const char** paths);

	void setDrawingEnabled(bool);

private:
	ZView *mRootView = nullptr;
	int mParentWidth = 0;
	int mParentHeight = 0;

	ZShader *mUIShader = nullptr;
	ZShader *mTextShader = nullptr;
	ZShader *mImageViewShader;

	bool mDrawingEnabled = true;

	string mResourcePath;

	// Shader code 
	const string ui_vs =
	#include "shaders/ui.vs"
	;

	const string ui_fs =
	#include "shaders/ui.fs"
	;

	const string text_vs =
	#include "shaders/text.vs"
	;

	const string text_fs =
	#include "shaders/text.fs"
	;

	const string image_fs =
	#include "shaders/imageview.fs"
	;
};

#endif