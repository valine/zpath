#ifndef ZCHECKBOX_H
#define ZCHECKBOX_H

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
    ZCheckbox(string label, ZView *parent);

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
    void setOnCheck(function<void(ZView *, bool)> onClick);
    void setTextColor(vec3 color) {
        mLabel->setTextColor(color);
    }

private:
	int debug;
	ZLabel* mLabel;
	ZView* mBox;
	ZView* mCheck;
	ZCheckboxListener* mListener = nullptr;
	bool mIsChecked = true;
    std::function<void(ZView*, float)> mClickListener = nullptr;

};

#endif