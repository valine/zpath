#include <utility>

#include <utility>

#include <ui/zscrollview.h>
#include <utils/zsettingsstore.h>
#include "ui/zdropdown.h"


ZDropDown::ZDropDown(float maxWidth, float maxHeight, vector<string> items, ZView *parent) : ZView(maxWidth, fillParent, parent) {

    mBackground = new ZView(maxWidth, maxHeight, this);
    mBackground->setBackgroundColor(grey1);
    mBackground->setCornerRadius(2);
    mButtonHeight = maxHeight;

    mTitle = new ZLabel("Dropdown", this);
    mTitle->setText("Dropdown");
    mTitle->setTextColor(grey1.getTextColor());
    mTitle->setMarginLeft(10);
    mTitle->setMaxWidth(maxWidth);
    mTitle->setYOffset((mBackground->getMaxHeight() - mTitle->getLineHeight()) / 2);
    mTitle->setOnClick([this](ZView* sender){
        handleClick();
    });

    addSubView(mTitle);

    mDrawer = new ZScrollView(maxWidth, 0, this);
    mDrawer->setBackgroundColor(bg);
    mDrawer->setOffset(0, maxHeight);
    mDrawer->setVisibility(false);
    mDrawer->setElevation(1.0);
    mDrawer->setAllowNegativeSize(true);
    mDrawer->setClippingEnabled(false);
    mDrawer->setCornerRadius(2);

    addSubView(mDrawer);

    mNames = items;
    int index = 0;
    for (const string& item : items) {
        auto* button = new ZButton(item, mDrawer);
        button->setIndexTag(index);
        button->setCornerRadius(0);
        button->setMaxWidth(getMaxWidth());
        button->setBackgroundColor(ZColor(vec4(1,1,1,0),
                                   vec4(0,0,0,0)));
        button->setClickMode(ZButton::ClickMode::upAndDown);
        button->setOnClick([this, item](ZView* sender){
            selectItem(sender->getIndexTag());

            if (mOnItemChange != nullptr) {
                mOnItemChange(sender->getIndexTag());
            }
            releaseFocus(this);
        });

        mButtons.push_back(button);
        index++;
    }
}

void ZDropDown::setItems(vector<string> items) {
    for (ZButton* button : mButtons) {
        button->setVisibility(false);
    }
    mNames = items;
    int index = 0;
    for (const string& item : items) {

        ZButton* button;
        if (mButtons.size() <= index) {
            button = new ZButton(item, mDrawer);
        } else {
            button = mButtons.at(index);
            button->setText(item);
            button->setVisibility(true);
        }

        button->setIndexTag(index);
        button->setCornerRadius(0);
        button->setBackgroundColor(ZColor(vec4(1,1,1,0),
                                          vec4(0,0,0,0)));
        button->setClickMode(ZButton::ClickMode::upAndDown);
        button->setOnClick([this, item](ZView* sender){
            selectItem(sender->getIndexTag());

            if (mOnItemChange != nullptr) {
                mOnItemChange(sender->getIndexTag());
            }
            releaseFocus(this);
        });

        mButtons.push_back(button);
        index++;
    }
}

void ZDropDown::wrapTitle() {
    mTitle->drawText();
    setMaxWidth(mTitle->getTextWidth() + MARGIN);
}

void ZDropDown::selectItem(int index) {
    mDrawer->setVisibility(false);
    getParentView()->invalidate();
    mSelected = index;
    if (mDynamicTitle) {
        mTitle->setText(mNames.at(index));
    }
}

void ZDropDown::setDynamicTitle(bool dynamic) {
    mDynamicTitle = dynamic;
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
        handleClick();
    }

	mDrawer->onMouseEvent(button,action,mods,x,y);
}

void ZDropDown::handleClick() {
    if (isMouseInBounds(mBackground)) {
        mDrawer->setVisibility(!mDrawer->getVisibility());
        mDrawer->scrollTo(0, 0);

        if (mDrawer->getVisibility() && mOnOpen != nullptr) {
            mOnOpen();
        }

        if (getGravity() == bottomLeft) {
            mDrawer->setMaxHeight(mDrawer->getInnerView()->getMaxHeight());
            mDrawer->setYOffset(mBackground->getMaxHeight());
        } else {
            mDrawer->setMaxHeight(std::min(getParentView()->getBottom() - mTitle->getTop() - (mBackground->getMaxHeight() * 2),
                          mDrawer->getInnerView()->getMaxHeight()));
            mDrawer->setYOffset(mBackground->getMaxHeight());
        }

        mDrawer->onWindowChange(getWindowWidth(), getWindowHeight());
        if (mDrawer->getVisibility()) {
            requestFocus(this);
        } else {
            releaseFocus(this);
        }
    } else {
        releaseFocus(this);
    }
}

void ZDropDown::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);
}

int ZDropDown::getMaxHeight() {
	ZView::getMaxHeight();
}
	
void ZDropDown::onCursorPosChange(double x, double y)  {
	 ZView::onCursorPosChange(x, y);
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

