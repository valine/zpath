#include "ui/zlistitemview.h"


ZListItemView::ZListItemView(float maxWidth, float maxHeight, ZView* parent) :
	ZView(maxWidth, maxHeight, parent) {

    setBackgroundColor(blue);

    mDropDown = new ZDropDown(100, 35, {"ONE", "TWO"}, this);
    mDropDown->setGravity(bottomLeft);
}