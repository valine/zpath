#include <utility>

#include <utility>

#include <ui/zscrollview.h>
#include <utils/zsettingsstore.h>
#include "ui/zdropdown.h"


ZDropDown::ZDropDown(float maxWidth, float maxHeight, vector<string> items, ZView *parent) : ZView(maxWidth, maxHeight, parent) {

    mBackground = new ZView(200, 25, this);
    mBackground->setBackgroundColor(grey);


    //setBackgroundColor(vec4(1,0,1,1));
    mButtonHeight = maxHeight;

    mTitle = new ZLabel("Dropdown", this);
    mTitle->setText("Dropdown");
    mTitle->setTextColor(vec3(0));
    mTitle->setMarginLeft(5);
    mTitle->setMarginTop(3);

    addSubView(mTitle);

    mDrawer = new ZScrollView(maxWidth, 0, this);
    mDrawer->setBackgroundColor(bg - vec4(0.05,0.05,0.05, 0.0));
    mDrawer->setOffset(0, DEFAULT_OFFSET);
    mDrawer->setVisibility(false);
    mDrawer->setElevation(1.0);
    mDrawer->setAllowNegativeSize(true);
    mDrawer->setClippingEnabled(false);

    addSubView(mDrawer);

    int index = 0;
    for (const string& item : items) {
        ZButton* button = new ZButton(item, mDrawer);
        button->setIndexTag(index);
        button->setBackgroundColor(vec4(0,0,0,0));
        button->getLabel()->setTextColor(ZSettingsStore::get().getBaseTextColor());
        button->setClickMode(ZButton::ClickMode::upAndDown);
        button->setOnClick([this, item](ZView* sender){
            mDrawer->setVisibility(false);
            getParentView()->invalidate();
            mTitle->setText(item);

            if (mOnItemChange != nullptr) {
                mOnItemChange(sender->getIndexTag());
            }
        });

        index++;
    }
}

void ZDropDown::setGravity(Gravity gravity) {
    ZView::setGravity(gravity);

    mTitle->setGravity(gravity);
    mBackground->setGravity(gravity);
    mDrawer->setGravity(gravity);
}


void ZDropDown::draw() {
	ZView::draw();
}

void ZDropDown::setTitle(string text) {
	mTitle->setText(std::move(text));
}

void ZDropDown::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);

	if (action == GLFW_PRESS) {
		if (isMouseInBounds(mBackground)) {
			mDrawer->setVisibility(!mDrawer->getVisibility());

            mDrawer->scrollTo(0,0);

            if (getGravity() == bottomLeft) {

                mDrawer->setMaxHeight(mDrawer->getInnerView()->getMaxHeight());
                mDrawer->setYOffset(DEFAULT_OFFSET);
			} else {
                mDrawer->setMaxHeight(std::min(getParentView()->getBottom() - mTitle->getTop() - DEFAULT_OFFSET,
                                               mDrawer->getInnerView()->getMaxHeight()));
                mDrawer->setYOffset(DEFAULT_OFFSET);
			}

            mDrawer->onWindowChange(getWindowWidth(), getWindowHeight());
			if (mDrawer->getVisibility()) {
			    requestFocus(this);
			} else {
			    releaseFocus(this);
			}
		}
	}

	if (action == GLFW_RELEASE) {
		// if (y < mTitle->getBottom()) {
		// 	mDrawer->setVisibility(!mDrawer->getVisibility());
		// }
	}
}

void ZDropDown::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);

}

int ZDropDown::getMaxHeight() {
	ZView::getMaxHeight();
//	return mButtonHeight;
}
	
void ZDropDown::onCursorPosChange(double x, double y)  {
	 ZView::onCursorPosChange(x, y);
	// if (mouseIsDown()) {
	// 	int deltaX =  x - getMouseDownX();
	// 	int deltaY = y - getMouseDownY();
	// }
}

void ZDropDown::onScrollChange(double x, double y) {
    ZView::onScrollChange(x, y);
    mDrawer->onScrollChange(x,y);
    mTitle->invalidate();
}

void ZDropDown::onGlobalMouseUp(int key) {
    if (!isMouseInBounds(mBackground)) {
        mDrawer->setVisibility(false);
        releaseFocus(this);
        getParentView()->invalidate();
    }
}

void ZDropDown::setOnItemChange(std::function<void(int item)> onItemChange) {
    mOnItemChange = std::move(onItemChange);
}

