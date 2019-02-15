#include "ui/zlinearlayout.h"

ZLinearLayout::ZLinearLayout(float maxWidth, float maxHeight) : 
ZView(maxWidth, maxHeight) {
	

}

void ZLinearLayout::addSubView(ZView* view) {
	ZView::addSubView(view);
	int y = 0;
	for (vector<ZView*>::iterator v = getSubViews().begin() ; v != getSubViews().end(); ++v) {
		ZView* subview = (*v);

        if (subview != nullptr) {
        	y+= subview->getMaxHeight();
      		subview->setYOffset(y);
        	
        }  
    }
}