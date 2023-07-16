
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
    ZApplication(ZViewController *controller, string name, bool shouldPoll, int windowWidth,
                 int windowHeight, string icon, function<void()> onStart);


    ZApplication(vector<ZViewController*> controllers);
    ZApplication(vector<ZViewController*> controllers, string name);
    ZApplication(vector<ZViewController*> controllers, string name, bool shouldPoll);
    ZApplication(vector<ZViewController*> controllers, string name, bool shouldPoll, int windowWidth, int windowHeight);

    ZApplication(ZViewController *controller, string name, bool shouldPoll, int windowWidth, int windowHeight,
                 string icon, function<void()> onStart, int fullScreen);

    ZApplication(ZViewController *controller, string name, int fullScreen);

    ZApplication(ZViewController *controller, string name, bool shouldPoll, int windowWidth, int windowHeight,
                 int fullScreen);

    void onWindowResize(int width, int height, ZViewController *viewController);
	void onWindowMove(GLFWwindow *window);
	void onKeyPress(int key, int scancode, int action, int mods, ZViewController *viewController);
	void onMouseEvent(GLFWwindow *window, int button, int action, int mods, double xpos, double ypos,
                      ZViewController *viewController);
	void onCursorPosChange(double x, double y, ZViewController *viewController);
	void onScrollEvent(GLFWwindow *window, double xoffset, double yoffset, ZViewController *viewController);
	void onFileDrop(GLFWwindow *window, int count, const char **paths, ZViewController *viewController);
	void setShouldPollEvents(bool);
private:

    string mIconPath = "";
    map<GLFWwindow*, ZViewController*> mWindows;
	vector<ZViewController*> mViewControllers;
	bool mShouldSwapBuffer = true;

    function<void()> mOnStartListener = nullptr;
	void init(vector<ZViewController *> controllers, string windowName, bool shouldPoll, int width, int height,
              int fullScreen, ZApplication *application);
	bool mShouldPoll = false;

	void startUiThread(ZViewController *viewController, bool shouldPoll, ZApplication *app, string windowName,
                       int width, int height, int fullScreen);

    void onCharacterInput(unsigned int character, ZViewController *viewController);
};