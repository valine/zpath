
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


    ZApplication(vector<ZViewController*> controllers);
    ZApplication(vector<ZViewController*> controllers, string name);
    ZApplication(vector<ZViewController*> controllers, string name, bool shouldPoll);
    ZApplication(vector<ZViewController*> controllers, string name, bool shouldPoll, int windowWidth, int windowHeight);

	void onWindowResize(int width, int height, ZViewController *viewController);
	void onWindowMove(GLFWwindow *window);
	void onKeyPress(int key, int scancode, int action, int mods, ZViewController *viewController);
	void onMouseEvent(GLFWwindow* window, int button, int action, int mods, double, double);
	void onCursorPosChange(double x, double y, ZViewController *viewController);
	void onScrollEvent(GLFWwindow *window, double xoffset, double yoffset, ZViewController *viewController);
	void onFileDrop(GLFWwindow *window, int count, const char **paths, ZViewController *viewController);
	void setShouldPollEvents(bool);
private:

    map<GLFWwindow*, ZViewController*> mWindows;
	vector<ZViewController*> mViewControllers;
	bool mShouldSwapBuffer = true;
	void init(vector<ZViewController *> controllers, string windowName, bool shouldPoll, int width, int height,
              ZApplication *application);
	bool mShouldPoll = false;

    static void startUiThread(ZViewController *viewController, bool shouldPoll, ZApplication *app, string windowName,
                              int width, int height);
};