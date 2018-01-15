
#include "ztiledview.h"


ZTiledView::ZTiledView(float maxWidth, float maxHeight, ZShader *shader) 
: ZView(maxWidth, maxHeight, shader) {


}


void ZTiledView::setTileCount(int x, int y) {
	mTileCountX = x;
	mTileCountY = y;

}