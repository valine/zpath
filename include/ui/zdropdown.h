#ifndef ZDROPDOWN_H
#define ZDROPDOWN_H

static const int DEFAULT_OFFSET = 25;

static const int MARGIN = 30;

#include "zview.h"
#include <iostream>
#include <map>
#include <string>
#include "zlabel.h"
#include "zlinearlayout.h"
#include "zscrollview.h"

using namespace std;

class ZDropDown : public ZView {
	
public:
    ZDropDown(float maxWidth, float maxHeight, vector<string> items, ZView* parent);

    void draw();
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
 	int getMaxHeight();
    void onScrollChange(double x, double y) override;
    void onGlobalMouseUp(int key) override;

    void setOnItemChange(std::function<void(int item)> onClick);
	void setTitle(string title);

    void setGravity(Gravity gravity) override;

    void selectItem(int index);
    void setDynamicTitle(bool dynamic);

    void wrapTitle();

    ZView* mBackground;
private:
	ZLabel* mTitle;
	ZScrollView* mDrawer;
	vector<string> mNames;
	float mButtonHeight;

	bool mDynamicTitle = true;
    std::function<void(int item)> mOnItemChange;

    void handleClick();
};

#endif