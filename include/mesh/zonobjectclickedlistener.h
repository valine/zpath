#ifndef ZONOBJECTCLICKEDLISTENER_H
#define ZONOBJECTCLICKEDLISTENER_H
#include "zobject.h"


class ZObject; 

// Interface definition
class ZOnObjectClickedListener {
public:
    ZOnObjectClickedListener();
    virtual void onClick(ZObject*);    
};

#endif
