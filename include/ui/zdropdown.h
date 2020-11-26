#ifndef ZDROPDOWN_H
#define ZDROPDOWN_H

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
    void onGlobalMouseUp() override;


    void setOnItemChange(std::function<void(int item)> onClick);
	void setTitle(string title);
private:
	ZLabel* mTitle;
	ZScrollView* mDrawer;
	float mButtonHeight;

    std::function<void(int item)> mOnItemChange;
};

#endif