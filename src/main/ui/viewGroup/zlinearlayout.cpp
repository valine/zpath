#include "ui/zlinearlayout.h"

ZLinearLayout::ZLinearLayout(float maxWidth, float maxHeight, ZView *parent) :
ZView(maxWidth, maxHeight, parent) {
	addSubView(new ZView(0,0));
}

ZLinearLayout::ZLinearLayout(float maxWidth, float maxHeight) :
        ZView(maxWidth, maxHeight) {
}

void ZLinearLayout::addSubView(ZView* view) {
	ZView::addSubView(view);
    view->setYOffset(mPos);
    mPos+=view->getMaxHeight() + view->getMarginTop() + view->getMarginBottom();
}