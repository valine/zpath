
#include <vector>
#include <string>

#define GLEW_STATIC

#include <GL/glew.h>
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

using glm::mat4;
using glm::ortho;
using std::vector;
using std::string;

class ZViewController : public ZOnClickListener {

public:
	ZViewController(string resourcePath);

	ZView* getRootView();

	void onWindowChange(int width, int height);
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
	void onScrollChange(double x, double y);
	void draw();

	void onClick(ZButton* sender);
private:
	ZView *mRootView = nullptr;
	int mParentWidth = 0;
	int mParentHeight = 0;

	ZShader *mUIShader = nullptr;
	ZShader *mTextShader = nullptr;

	ZTiledView *mTileView = nullptr;

	ZButton *mGridViewButton;
	ZButton *mIncrementButton;
	ZButton *mDecrementButton;
	ZLabel *mExposureLabel;

	ZButton *mRoughnessIncrementButton;
	ZButton *mRoughnessDecrementButton;
	ZLabel *mRoughnessLabel;

	ZButton *mMetalIncrementButton;
	ZButton *mMetalDecrementButton;
	ZLabel *mMetalLabel;

	bool mIsQuadView = true;

};