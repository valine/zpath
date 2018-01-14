
#include <string> 

#include "zview.h"
#include "zviewcontroller.h"

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ZApplication {
	
public: 
	std::string mDataPath;
	ZApplication(std::string dataPath);
	void onWindowChange(int width, int height);

private:
	ZView *view;
	ZViewController *viewController;

};