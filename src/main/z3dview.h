#include "zview.h"
#include <iostream>
#include <zshader.h>

using std::cout;
using std::endl;

class Z3DView : public ZView {

public:
	Z3DView(float maxWidth, float maxHeight, int debug);

	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
	void draw();
private:
	int mDebug; 
};