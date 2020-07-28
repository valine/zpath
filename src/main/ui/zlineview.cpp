//
// Created by lukas on 7/27/20.
//

#include "ui/zlineview.h"


ZLineView::ZLineView(float maxWidth, float maxHeight, ZView *parent) : ZView(maxWidth, maxHeight, parent) {
    init();
}

void ZLineView::init() {
    setBackgroundColor(vec4(1,0,0,1));

}
