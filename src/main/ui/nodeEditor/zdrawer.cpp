//
// Created by lukas on 3/10/21.
//

#include "ui/zdrawer.h"

ZDrawer::ZDrawer(ZView *parent) : ZScrollView(20, fillParent, parent) {
    setBackgroundColor(bg);
    setElevation(0.5);

    new ZView(vec2(200), grey, this);
    new ZView(vec2(100), grey, this);
    new ZView(vec2(200), grey, this);
}
