//
// Created by lukas on 3/10/21.
//

#include "ui/zdrawer.h"

ZDrawer::ZDrawer(ZView *parent) : ZScrollView(100, fillParent, parent) {
    setBackgroundColor(blue);
    setElevation(0.5);
    new ZView(vec2(200), red, this);
    new ZView(vec2(100), green, this);
    new ZView(vec2(200), yellow, this);
}
