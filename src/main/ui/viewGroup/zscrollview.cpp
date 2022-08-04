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
	mScrollBar->setBackgroundColor(vec4(0.5,0.5,0.5,0.3));
	mScrollBar->setGravity(ZView::topRight);

	mInnerView = new ZLinearLayout(fillParent, getHeight());
	addSubView(mInnerView);
	mInnerView->setMargin(0,0,7,0);

	setBackgroundColor(bg);
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
		if (mDynamicSize) {
            mInnerView->setMaxHeight(view->getOffsetY() + view->getMaxHeight() +
                                     view->getMarginTop() + view->getMarginBottom());
        }
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

ZLinearLayout* ZScrollView::getInnerView() {
    return mInnerView;
}

void ZScrollView::onScrollChange(double x, double y) {

    if (shiftKeyPressed()) {
        ZView::onScrollChange(x,y);
    } else {
        if (mEnableScroll) {
            mScrollBar->setVisibility(mInnerView->getHeight() >= getHeight());
            int scrollSpeed = 50;
            double scrollBottom = mInnerView->getTranslation().y + mInnerView->getMaxHeight();

            double distance = getHeight() - scrollBottom;
            if (y < 0) { // Scrolling down
                if (scrollBottom > getHeight()) {
                    mInnerView->translateBy(vec2(0, std::max(distance, y * scrollSpeed)));
                }
            } else { // scrolling up
                if (mInnerView->getTranslation().y < 0) {
                    double current = mInnerView->getTranslation().y;
                    mInnerView->translateBy(vec2(0, std::min(-current, y * scrollSpeed)));
                }
            }

            mInnerView->onWindowChange(getWidth(), getHeight());

            auto innerHeight = (float) mInnerView->getMaxHeight();
            auto viewHeight = (float) getHeight();
            float scrollBarHeight = viewHeight * (viewHeight / innerHeight);

            mScrollBar->setMaxHeight(scrollBarHeight);
            float max = viewHeight - scrollBarHeight;
            float factor = -(mInnerView->getTranslation().y) / (innerHeight - getHeight());
            mScrollBar->setOffset(0, (max * factor));

            invalidate();
        }
    }
}

void ZScrollView::scrollTo(int x, int y) {
    mInnerView->setTranslation(vec2(x,y));
}

void ZScrollView::onWindowChange(int windowWidth, int windowHeight) {
    ZView::onWindowChange(windowWidth, windowHeight);

    onScrollChange(0,0);
}
