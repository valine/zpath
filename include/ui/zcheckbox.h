#ifndef ZCHECKBOX_H
#define ZCHECKBOX_H

#include "zview.h"
#include <iostream>
#include <map>
#include <string>

#include "zlabel.h"
#include "zcheckboxlistener.h"

using namespace std;

class ZCheckboxListener;

class ZCheckbox : public ZView {
	
public:

	ZCheckbox(float maxWidth, float maxHeight, string resources);
	void draw();
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
	void setText(string text);
	void setListener(ZCheckboxListener* listener);
	void setBoxBackground(ZTexture*);
	void setCheckBackground(ZTexture*);
	void setChecked(bool);
private:
	int debug;
	ZLabel* mLabel;
	ZView* mBox;
	ZView* mCheck;


	ZCheckboxListener* mListener = nullptr;

	bool mIsChecked = true;
};

#endif