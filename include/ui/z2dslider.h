#ifndef ZSLIDER2D_H
#define ZSLIDER2D_H

static const int LINE_WIDTH = 2;

#include <string>
#include <iostream>
#include "zview.h"
#include "mesh/zpath_constants.h"
#include "zsliderlistener.h"
#include "zlabel.h"
#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace ZPathConstants;

class Z2DSlider : public ZView {

public:
    Z2DSlider(float maxWidth, float maxHeight, vec2 min, vec2 max, ZView *parent);

	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);

	void setMaxValue(vec2 max);
	void setMinValue(vec2 min);

	void setValue(vec2 value);
	void setIncrement(float);
	vec2 getValue();
    void setOnSlide(function<void(ZView*, vec2, bool)> onSlide);
    void setTextColor(ZColor color);
private:
    function<void(ZView* sender, vec2 value, bool)> mSlideListener;

	ZView* mLineH;
    ZView* mLineV;
	string mTitle;

	vec2 mMaxValue = vec2(1.0);
	vec2 mMinValue = vec2(0.0);

	ZLabel* mLabel;
	float mIncrement = 0.01;
    vec2 mValue = vec2(0);

	void valueChanged(float x, float y);
};

#endif ZSLIDER2D_H