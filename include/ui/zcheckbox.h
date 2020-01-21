#ifndef ZCHECKBOX_H
#define ZCHECKBOX_H

static const int CB_WIDTH = 10000;

static const int CB_HEIGHT = 20;

#include "zview.h"
#include <iostream>
#include <map>
#include <string>

#include "zlabel.h"
#include "zcheckboxlistener.h"
#include <functional>

using namespace std;

class ZCheckboxListener;

class ZCheckbox : public ZView {
	
public:

	ZCheckbox(float maxWidth, float maxHeight, string resources);

    ZCheckbox(string label, string resourcePath, ZView *parent);

    void draw();
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
	void setText(string text);
	void setListener(ZCheckboxListener* listener);
	void setBoxBackground(ZTexture*);
	void setCheckBackground(ZTexture*);
	void setChecked(bool);

    void setCheckColor(vec4 color);
    void setOnClick(function<void(ZView*, bool)>);

private:
	int debug;
	ZLabel* mLabel;
	ZView* mBox;
	ZView* mCheck;
	ZCheckboxListener* mListener = nullptr;
	bool mIsChecked = true;
    function<void(ZView*, float)> mClickListener;

};

#endif