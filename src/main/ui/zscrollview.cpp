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
	mScrollBar = new ZView(7,100);
	mScrollBar->setOffset(3,0);
	mScrollBar->setBackgroundColor(vec4(0.5,0.5,0.5,1));
	mScrollBar->setGravity(ZView::bottomRight);
	addSubView(mScrollBar);

	mInnerView = new ZView(getWidth(), getHeight());
	addSubView(mInnerView);
}

void ZScrollView::addSubView(ZView* view) {
	if (view == mInnerView ||
		view == mScrollBar) {
		ZView::addSubView(view);
	} else {
		mInnerView->addSubView(view);
	}
}

void ZScrollView::setInnerViewHeight(int height) {
	mInnerView->setMaxHeight(height);
}

void ZScrollView::setInnerViewWidth(int width) {
	mInnerView->setMaxWidth(width);
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
	int scrollSpeed = 50;
	int scrollBottom = mInnerView->getOffsetY() + mInnerView->getMaxHeight();

	if (y < 0) { // Scrolling down
		if (scrollBottom > getHeight()) {
			mInnerView->offsetBy(0, y * scrollSpeed);
		}
	} else { // scrolling up 
		if (mInnerView->getOffsetY() < 0) {
			mInnerView->offsetBy(0, y * scrollSpeed);
		}
	}

	mInnerView->onWindowChange(getWidth(), getHeight());
	float innerHeight = (float) mInnerView->getMaxHeight();
	float viewHeight = (float) getHeight();
	float scrollBarHeight = viewHeight * (viewHeight / innerHeight);

	float scrollBarOffset = mInnerView->getOffsetY() / (viewHeight - scrollBarHeight);
	
	cout<<scrollBarOffset<<endl;

	//if (mInnerView->getMaxHeight() > getHeight()) {
		mScrollBar->setMaxHeight(scrollBarHeight);
		mScrollBar->setOffset(0, viewHeight - scrollBarHeight);
	//}

	invalidate();
}