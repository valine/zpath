#ifndef ZONCLICKLISTENER_H
#define ZONCLICKLISTENER_H
#include "ui/zbutton.h"


class ZButton; 

// Interface definition
class ZOnClickListener {
public:
    ZOnClickListener();
    virtual void onClick(ZButton*);    
};

#endif
