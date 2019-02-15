#include "ui/zlinearlayout.h"

ZLinearLayout::ZLinearLayout(float maxWidth, float maxHeight) : 
ZView(maxWidth, maxHeight) {
	

}

void ZLinearLayout::addSubView(ZView* view) {
	ZView::addSubView(view);

}