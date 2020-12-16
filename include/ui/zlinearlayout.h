#ifndef ZLINEARLAYOUT_H
#define ZLINEARLAYOUT_H

#include "zview.h"
#include <iostream>
#include <map>
#include <string>

#include "zlabel.h"
#include "zonclicklistener.h"

using namespace std;

class ZLinearLayout : public ZView {


public:
	ZLinearLayout(float maxWidth, float maxHeight, ZView *parent);
    ZLinearLayout(float maxWidth, float maxHeight);
	void addSubView(ZView* view);
	

private:
	float mPos = 0;

};

#endif