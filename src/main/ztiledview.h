#include "zview.h"


#include <iostream>
using std::cout;
using std::endl;

class ZTiledView : public ZView {

public:

	ZTiledView(float maxWidth, float maxHeight, int tilesX, int tilesY);
	void setTileCount(int x, int y);
	void computeBounds(int windowHeight, int maxWidth);
	void setTileMargin(int margin);

	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
private:
	int mTileCountX;
	int mTileCountY;

	int mTileMargin = 2;
	
	int mInitialTileWeightX = 0;
	int mTileWeightX = 0;
};