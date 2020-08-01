#include "ui/zlinearlayout.h"

ZLinearLayout::ZLinearLayout(float maxWidth, float maxHeight) : 
ZView(maxWidth, maxHeight) {
	addSubView(new ZView(0,0));
}

void ZLinearLayout::addSubView(ZView* view) {
	ZView::addSubView(view);
    view->setYOffset(mPos);
    mPos+=view->getMaxHeight() + view->getMarginTop() + view->getMarginBottom();
}