#ifndef Z3DVIEW_H
#define Z3DVIEW_H

#include "zview.h"
#include <iostream>
#include <ui/zshader.h>
#include "mesh/zobjloader.h"
#include "mesh/zrenderer.h"
#include "mesh/zscene.h"
#include <vector> 
#include <string>

using std::cout;
using std::endl;

class ZRenderer;

class Z3DView : public ZView {

public:
	Z3DView(float maxWidth, float maxHeight, ZRenderer *renderer);
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
	void draw();
	void setRenderer(ZRenderer *renderer);
private:
	ZRenderer *mRenderer;
};

#endif