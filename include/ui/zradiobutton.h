#ifndef ZRADIOBUTTON_H
#define ZRADIOBUTTON_H

#include "zview.h"
#include <iostream>
#include <map>
#include <string>
#include <utils/zsettingsstore.h>

#include "zlabel.h"
#include "zonclicklistener.h"
#include "ztexture.h"

using namespace std;

class ZOnClickListener;

class ZRadioButton : public ZView, public ZOnClickListener {
	
public:

	ZRadioButton(float maxWidth, float maxHeight, string resource, vector<string> titles);
	ZRadioButton(float maxWidth, float maxHeight, vector<ZTexture *> backgrounds, vector<string> tags);

    ZRadioButton(string label, vector<string> titles, ZView *parent);

    void draw();
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
	void setText(string text);
	void setVertical(bool);

	void setOnClickListener(ZOnClickListener* listener);
	void computeBounds(int windowHeight, int maxWidth); 
	void onClick(ZButton* sender);
	void setSpacing(int spacing);
	void showLabel(bool);
	void setHighlightColor(vec4);
	void setBaseColor(vec4);
	ZButton* getButton(int index);
	void setOnClick(function<void(ZView*, int)>);
private:
	int debug;

	ZButton* mHighlighted;
	int mSpacing = 0;
	ZLabel* mLabel;
	ZOnClickListener* mListener = nullptr;
	vector<string> mTitles;
	bool mIsVertical = false;
	vec4 mHighlightColor = ZSettingsStore::getInstance().getHighlightColor();
	vec4 mBaseColor = ZSettingsStore::getInstance().getBaseColor();
	vector<ZButton*> mButtons;

    void init(float maxWidth, float maxHeight, vector<string> &titles);
    function<void(ZView*, int)> mOnClick;
};

#endif