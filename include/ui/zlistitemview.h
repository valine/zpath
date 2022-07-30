#ifndef ZLISTITEMVIEW_H
#define ZLISTITEMVIEW_H

#include "zview.h"
#include <iostream>
#include <map>
#include <string>
#include <string>

#include "zdropdown.h"
#include "zlabel.h"

using namespace std;

class ZListItemView : public ZView {
	
public:

	ZListItemView(float maxWidth, float maxHeight, ZView* parent);

    ZDropDown* mDropDown;
    ZLabel* mTitle;

private:



};

#endif