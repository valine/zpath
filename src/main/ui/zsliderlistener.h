#ifndef ZSLIDERLISTENER_H
#define ZSLIDERLISTENER_H
#include "ui/zslider.h"


class ZSlider; 

// Interface definition
class ZSliderListener {
public:
    ZSliderListener();
    virtual void onSliderValueChanged(ZSlider*, float);    
};

#endif
