#include "zview.h"

#include <iostream>
using std::cout;
using std::endl;

class ZTiledView : public ZView {

public:

	ZTiledView(float maxWidth, float maxHeight, ZShader *shader);
	void setTileCount(int x, int y);
private:
	int mTileCountX;
	int mTileCountY;

};