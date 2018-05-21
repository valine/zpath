#include "ui/zscrollview.h"


ZScrollView::ZScrollView(float maxWidth, float maxHeight):
ZView(maxWidth, maxHeight) {
	init();
}

ZScrollView::ZScrollView(Bounds maxWidth, float maxHeight):
ZView(maxWidth, maxHeight) {
	init();
}

ZScrollView::ZScrollView(float maxWidth, Bounds maxHeight):
ZView(maxWidth, maxHeight) {
	init();
}

ZScrollView::ZScrollView(Bounds maxWidth, Bounds maxHeight):
ZView(maxWidth, maxHeight) {
	init();
}

void ZScrollView::init() {
	mScrollBar = new ZView(100,100);
	//mScrollBar->setBackgroundColor(vec4(1,0,0,1));
	addSubView(mScrollBar);

	mInnerView = new ZView(getWidth(), getHeight());
	//mInnerView->setBackgroundColor(vec4(0,1,0,1));
	addSubView(mInnerView);
}


void ZScrollView::onKeyPress(int key, int scancode, int action, int mods){
	ZView::onKeyPress(key, scancode, action, mods);

}

void ZScrollView::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);
}

void ZScrollView::onCursorPosChange(double x, double y) {
	ZView::onCursorPosChange(x, y);

}

void ZScrollView::onScrollChange(double x, double y) {
	ZView::onScrollChange(x,y);

}