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
private:
	int mTileCountX;
	int mTileCountY;

	int mTileMargin = 2;
};