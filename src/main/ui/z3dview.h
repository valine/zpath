#ifndef Z3DVIEW_H
#define Z3DVIEW_H

#define GLEW_STATIC

#include <GL/glew.h>
#include "zview.h"
#include <iostream>
#include <ui/zshader.h>
#include "mesh/zobjloader.h"
#include "mesh/zrenderer.h"
#include "mesh/zscene.h"
#include <vector> 
#include <string>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
using namespace glm;

using std::cout;
using std::endl;

class ZRenderer;

class Z3DView : public ZView {

public:
	Z3DView(float maxWidth, float maxHeight, ZRenderer *renderer);
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
	void onScrollChange(double x, double y);
	void draw();
	void setRenderer(ZRenderer *renderer);
private:
	ZRenderer *mRenderer;

	float mRotationX = 0;
	float mRotationY = 0;

	void updateCameraPosition();

	vec3 mOrbitAnchorPoint = vec3(5,0,0);
};

#endif