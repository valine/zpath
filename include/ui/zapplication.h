
#include <string> 
#include "glad/glad.h"
#include "zview.h"
#include "zviewcontroller.h"
#include <GLFW/glfw3.h>

using std::string;

class ZApplication {
	
public: 
	ZApplication(ZViewController* viewcontroller);
	ZApplication(ZViewController* viewcontroller, string windowName);
	ZApplication(ZViewController* controller, string name, bool shouldPoll);
	ZApplication(ZViewController* controller, string name, bool shouldPoll, int windowWidth, int windowHeight);

	void onWindowResize(int width, int height);
	void onWindowMove(GLFWwindow *window);
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(GLFWwindow* window, int button, int action, int mods);
	void onCursorPosChange(double x, double y);
	void onScrollEvent(GLFWwindow *window, double xoffset, double yoffset);
	void onFileDrop(GLFWwindow *window, int count, const char** paths);
	void setShouldPollEvents(bool);
private:
	ZViewController *mViewController;
	bool mShouldSwapBuffer = true;
	void init(ZViewController* viewcontroller, string windowName,
		bool shouldPoll, int width, int height);

	bool mShouldPoll = false;
};