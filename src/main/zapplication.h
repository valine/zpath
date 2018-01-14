
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
	void onWindowChange(int width, int height);

private:
	ZViewController *viewController;
};