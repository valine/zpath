#include "zview.h"

class ZButton : public ZView {
	
public:

	ZButton(float maxWidth, float maxHeight);
	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
private:



};