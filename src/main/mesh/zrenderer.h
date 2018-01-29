#ifndef ZRENDERER_H
#define ZRENDERER_H

#include <string>
#include "zscene.h"
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using std::string;

class ZRenderer {

public:
	ZRenderer(string debug);

	void setScene(ZScene *scene);
	void draw();	

private:
	ZScene *mScene;
};

#endif
