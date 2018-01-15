
#include "ztiledview.h"


ZTiledView::ZTiledView(float maxWidth, float maxHeight) 
: ZView(maxWidth, maxHeight) {


}


void ZTiledView::setTileCount(int x, int y) {
	mTileCountX = x;
	mTileCountY = y;

}