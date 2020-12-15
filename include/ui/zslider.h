#ifndef ZSLIDER_H
#define ZSLIDER_H

static const int Y_OFFSET = 10;

#include <string>
#include <iostream>
#include "zview.h"
#include "mesh/zpath_constants.h"
#include "zsliderlistener.h"
#include "zlabel.h"
#include <functional>
using namespace std;
using namespace ZPathConstants;

class ZSliderListener;


class ZSlider : public ZView {

public:
	ZSlider(float maxWidth, float maxHeight, string title);
	ZSlider(float maxWidth, float maxHeight, string title, string resourcePath);

    ZSlider(string label, float min, float max, float value, ZView *parent);

    void onKeyPress(int key, int scancode, int action, int mods);
	bool onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
	void onScrollChange(double x, double y);
	void setListener(ZSliderListener* listener);

	void setMaxValue(float max);
	void setMinValue(float min);

	void setValue(float value);
	void setLineColor(vec4 color);
	void setThumbColor(vec4 color);
	void setIncrement(float);
	void setTextColor(vec3 color);
	void setFillColor(vec4 color);

	void setHighlightBackground(ZTexture* tex);
	
	void setThumbBackground(ZTexture* tex);
	float getValue();
    void setOnSlide(function<void(ZView*, float, bool)> onSlide);


private:
    function<void(ZSlider* sender, float value, bool)> mSlideListener;

	ZView* mThumb;
	ZView* mLine;
	ZView* mHighlight;
	string mTitle;

	float mMaxValue = 1.0;
	float mMinValue = 0.0;

	ZSliderListener* mListener = nullptr;
	ZLabel* mLabel;
	float mIncrement = 0.01;
	float mValue = 0.0;

	void valueChanged(float offset);
};

#endif