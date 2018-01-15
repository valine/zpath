
#include "ztiledview.h"


ZTiledView::ZTiledView(float maxWidth, float maxHeight, ZShader *shader) 
: ZView(maxWidth, maxHeight, shader) {


}


void ZTiledView::sayHi() {
	cout<<"hi tiled view"<<endl;
}