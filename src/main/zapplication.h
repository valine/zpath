
#include <string> 

#include "zview.h"
#include "zviewcontroller.h"

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using std::string;

class ZApplication {
	
public: 
	ZApplication(string resourcePath);
	void onWindowResize(int width, int height);
	void onWindowMove(GLFWwindow *window);
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods);
	void onCursorPosChange(double x, double y);

private:
	ZViewController *viewController;
	bool mShouldSwapBuffer = true;
};