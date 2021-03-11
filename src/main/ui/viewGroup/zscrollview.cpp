#include <ui/zlinearlayout.h>
#include <utils/zsettingsstore.h>
#include "ui/zscrollview.h"


ZScrollView::ZScrollView(float maxWidth, float maxHeight, ZView* parent):
        ZView(maxWidth, maxHeight, parent) {
    init();
}

ZScrollView::ZScrollView(float maxWidth, float maxHeight):
        ZView(maxWidth, maxHeight) {
    init();
}

void ZScrollView::init() {
	mScrollBar = new ZView(7,100);
	addSubView(mScrollBar);
	mScrollBar->setOffset(3,0);
	mScrollBar->setBackgroundColor(vec4(0.5,0.5,0.5,1));
	mScrollBar->setGravity(ZView::topRight);

	mInnerView = new ZLinearLayout(getWidth(), getHeight());
	addSubView(mInnerView);
	mInnerView->setMargin(0,0,7,0);

	setBackgroundColor(ZSettingsStore::getInstance().getBackgroundColor());
	setInnerViewHeight(1000);

	mInnerView->resetCursor();
}

void ZScrollView::addSubView(ZView* view) {
	if (view == mInnerView ||
		view == mScrollBar ||
		!view->mIsScrollable ||
		mInnerView == nullptr) {
		ZView::addSubView(view);
	} else {
		mInnerView->addSubView(view);
		mInnerView->setMaxHeight(view->getOffsetY() + view->getMaxHeight() +
		view->getMarginTop() + view->getMarginBottom());
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

bool ZScrollView::onMouseEvent(int button, int action, int mods, int x, int y) {
	return ZView::onMouseEvent(button, action, mods, x, y);
}

void ZScrollView::onCursorPosChange(double x, double y) {
	ZView::onCursorPosChange(x, y);
}

ZView* ZScrollView::getInnerView() {
    return mInnerView;
}

void ZScrollView::onScrollChange(double x, double y) {

    mScrollBar->setVisibility(mInnerView->getHeight() >= getHeight());
	int scrollSpeed = 50;
	double scrollBottom = mInnerView->getOffsetY() + mInnerView->getMaxHeight();

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
	auto innerHeight = (float) mInnerView->getMaxHeight();
    auto viewHeight = (float) getHeight();
	float scrollBarHeight = viewHeight * (viewHeight / innerHeight);

    mScrollBar->setMaxHeight(scrollBarHeight);
    float max = viewHeight - scrollBarHeight;
    float factor = -(mInnerView->getOffsetY()) / (innerHeight - getHeight());
    mScrollBar->setOffset(0, (max * factor));

	invalidate();
}

void ZScrollView::onWindowChange(int windowWidth, int windowHeight) {
    ZView::onWindowChange(windowWidth, windowHeight);

    onScrollChange(0,0);
}
