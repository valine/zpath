#include "zview.h"

#include <iostream>
#include <map>
#include <string>
#include <string>

#include "zlabel.h"
#include "zonclicklistener.h"

using namespace std;
  

class ZButton : public ZView {
	
public:

	ZButton(float maxWidth, float maxHeight, string title);
	void draw();
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);

	void setText(string text);

	void setOnClickListener(ZOnClickListener* listener);
private:
int debug;

	ZLabel* mLabel;
	ZOnClickListener* mListener = nullptr;


};