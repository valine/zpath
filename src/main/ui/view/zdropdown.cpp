#include <utility>

#include <utility>

#include <ui/zscrollview.h>
#include <utils/zsettingsstore.h>
#include "ui/zdropdown.h"


ZDropDown::ZDropDown(float maxWidth, float maxHeight, vector<string> items, ZView *parent) : ZView(maxWidth, maxHeight, parent) {

    //setBackgroundColor(vec4(1,0,1,1));
    mButtonHeight = maxHeight;

    mTitle = new ZLabel("Dropdown", this);
    mTitle->setText("Dropdown");
    mTitle->setTextColor(vec3(1));
    addSubView(mTitle);

    mDrawer = new ZScrollView(400, 0, this);
    mDrawer->setBackgroundColor(ZSettingsStore::get().getBackgroundColor());
    mDrawer->setOffset(0,30);
    mDrawer->setVisibility(false);
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

        mDrawer->setMaxHeight(mDrawer->getInnerView()->getMaxHeight());

        index++;
    }
}


void ZDropDown::draw() {
	ZView::draw();
}

void ZDropDown::setTitle(string text) {
	mTitle->setText(std::move(text));
}

bool ZDropDown::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);

	if (action == GLFW_PRESS) {
		if (y < mTitle->getBottom()) {
			mDrawer->setVisibility(!mDrawer->getVisibility());
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
    if (!isMouseInBounds(mTitle)) {
        mDrawer->setVisibility(false);
        getParentView()->invalidate();
    }
}

void ZDropDown::setOnItemChange(std::function<void(int item)> onItemChange) {
    mOnItemChange = std::move(onItemChange);
}

