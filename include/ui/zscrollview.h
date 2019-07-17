#ifndef ZSCROLLVIEW_H
#define ZSCROLLVIEW_H

#include <string> 
#include <iostream>
#include "zview.h"
#include "mesh/zpath_constants.h"

using namespace std;
using namespace ZPathConstants;


class ZScrollView : public ZView {

public:

	ZScrollView(float maxWidth, float maxHeight);
	ZScrollView(Bounds maxWidth, float maxHeight);
	ZScrollView(float maxWidth, Bounds maxHeight);
	ZScrollView(Bounds maxWidth, Bounds maxHeight);


	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
	void onScrollChange(double x, double y);
	void addSubView(ZView* view);
	void setInnerViewHeight(int height);
	void setInnerViewWidth(int width);
	void onWindowChange(int windowWidth, int windowHeight);
	
private:

	void init();

	ZView* mScrollBar;
	ZView* mScrollBarBackground;
	ZView* mInnerView;
};

#endif