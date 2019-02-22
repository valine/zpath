#ifndef ZRADIOBUTTON_H
#define ZRADIOBUTTON_H

#include "zview.h"
#include <iostream>
#include <map>
#include <string>

#include "zlabel.h"
#include "zonclicklistener.h"

using namespace std;

class ZOnClickListener;

class ZRadioButton : public ZView, public ZOnClickListener {
	
public:

	ZRadioButton(float maxWidth, float maxHeight, string resource, vector<string> titles);
	void draw();
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);

	void setText(string text);

	void setOnClickListener(ZOnClickListener* listener);
	void computeBounds(int windowHeight, int maxWidth); 
	void onClick(ZButton* sender);
private:
int debug;

	ZLabel* mLabel;
	ZOnClickListener* mListener = nullptr;
	vector<string> mTitles;

};

#endif