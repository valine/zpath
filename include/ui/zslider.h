#ifndef ZSLIDER_H
#define ZSLIDER_H

#include <string> 
#include <iostream>
#include "zview.h"
#include "mesh/zpath_constants.h"
#include "zsliderlistener.h"
#include "zlabel.h"

using namespace std;
using namespace ZPathConstants;

class ZSliderListener;


class ZSlider : public ZView {

public:
	ZSlider(float maxWidth, float maxHeight, string title);
	ZSlider(float maxWidth, float maxHeight, string title, string resourcePath);
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
	void onScrollChange(double x, double y);
	void setListener(ZSliderListener* listener);

	void setMaxValue(float max);
	void setMinValue(float min);

	void setValue(float value);
	void setLineColor(vec4 color);
	void setThumbColor(vec4 color);
	void setIncrement(float);
	
private:
	ZView* mThumb;
	ZView* mLine;
	ZView* mHighlight;
	string mTitle;

	float mMaxValue = 1.0;
	float mMinValue = 0.0;

	ZSliderListener* mListener = nullptr;
	ZLabel* mLabel;
	float mIncrement = 0.01;

	void valueChanged(float offset);
};

#endif