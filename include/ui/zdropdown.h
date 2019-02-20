#ifndef ZDROPDOWN_H
#define ZDROPDOWN_H

#include "zview.h"
#include <iostream>
#include <map>
#include <string>
#include "zlabel.h"
#include "zlinearlayout.h"

using namespace std;

class ZDropDown : public ZView {
	
public:

	ZDropDown(float maxWidth, float maxHeight, string items[], string resourcePath);
	void draw();
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
 	int getMaxHeight();
	void setTitle(string title);
private:
	ZLabel* mTitle;
	ZLinearLayout* mDrawer;
	float mButtonHeight;

};

#endif