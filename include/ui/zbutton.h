#ifndef ZBUTTON_H
#define ZBUTTON_H

#define BTN_LABEL_HEIGHT 19
#define BTN_MARGIN 0
#define BTN_OFFSET 0

#define DEFAULT_WIDTH 140
#define DEFAULT_HEIGHT 22

#include "zview.h"
#include <iostream>
#include <map>
#include <string>

#include "zlabel.h"
#include "zonclicklistener.h"
#include <functional>
#include "mesh/zpath_constants.h"

using namespace std;
using namespace ZPathConstants;

class ZOnClickListener;

class ZButton : public ZView {
	
public:
    enum ClickMode {
        upAndDown,
        onlyUp
    };


	ZButton(float maxWidth, float maxHeight);
    ZButton(string label, ZView *parent);

    void draw();
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
    void setOnClick(std::function<void(ZView* sender)> onClick) override;
	void onCursorPosChange(double x, double y);
	void setText(string text);
	string getText();
	void setOnClickListener(ZOnClickListener* listener);
	void computeBounds();
    void setOnClick(std::function<void()> onClick);
    void setClickMode(ClickMode clickMode);
    void onSizeChange() override;

    ZLabel *getLabel();

    void setBackgroundColor(ZColor color);

private:
int debug;
	ZLabel* mLabel;
	ZOnClickListener* mListener = nullptr;
	bool mWasPressed = false;

	// Less code to setup. Works well when you only have a single button.
    std::function<void()> mOnClickSimple;

	ClickMode mClickMode = upAndDown;

    void onMouseOver() override;
    void onMouseLeave() override;

};

#endif