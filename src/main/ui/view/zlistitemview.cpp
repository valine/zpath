#include "ui/zlistitemview.h"


ZListItemView::ZListItemView(float maxWidth, float maxHeight, ZView* parent) :
	ZView(maxWidth, maxHeight, parent) {

    setBackgroundColor(grey1);

    mTitle = new ZLabel("", this);
    mTitle->setMargin(5);

    mDropDown = new ZDropDown(100, 30, {"ONE", "TWO"}, this);
    mDropDown->setMargin(2);
    mDropDown->setGravity(bottomRight);


}