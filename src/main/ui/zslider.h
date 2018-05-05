#ifndef ZSLIDER_H
#define ZSLIDER_H

#include <string> 
#include <iostream>
#include "zview.h"
#include "mesh/zpath_constants.h"

using namespace std;
using namespace ZPathConstants;

class ZSlider : public ZView {

public:

	ZSlider(float maxWidth, float maxHeight, string title);
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
	void onScrollChange(double x, double y);

private:

	ZView* mThumb;
	ZView* mLine;

};

#endif