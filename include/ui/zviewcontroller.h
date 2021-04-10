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
	explicit ZViewController(string resourcePath);
    explicit ZViewController(char* argv[]);
    explicit ZViewController(const char* argv[]);

    string getResourcePath();

    // Life cycle events driven by application
	void onCreate() override;
    void draw() override;
    virtual void onTerminate();

	// Glfw window events
    void onWindowChange(int width, int height) override;
    void onKeyPress(int key, int scancode, int action, int mods) override;
    void onMouseEvent(int button, int action, int mods, int x, int y) override;
    void onCursorPosChange(double x, double y) override;
    void onScrollChange(double x, double y) override;
	void onFileDrop(int count, const char** paths) override;

    void onCharacterInput(unsigned int character) override;

	bool isViewInFocus() override;
	void requestFocus(ZView* view) override;
    void releaseFocus(ZView *forView) override;
    ZView* getFocusedView() override;

	void setDrawingEnabled(bool);
private:
	int mParentWidth = 0;
	int mParentHeight = 0;

	vec2 mLastClickPosition = vec2(0);
    long mLastClickTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    long mLastDoubleClickTime = 0;


    ZShader *mUIShader = nullptr;
	ZShader *mTextShader = nullptr;
	ZShader *mImageViewShader;

	bool mDrawingEnabled = true;

	ZView* mFocusedView = nullptr;

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