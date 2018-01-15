
#include <vector>
#include <string>

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "zview.h"
#include "zshader.h"
#include "ztiledview.h"

using glm::mat4;
using glm::ortho;
using std::vector;
using std::string;

class ZViewController {

public:
	ZViewController(string resourcePath);

	ZView* getRootView();

	void onWindowChange(int width, int height);

	void draw();
private:
	ZView *mRootView;
	int mParentWidth;
	int mParentHeight;

	ZShader *mUIShader;

};