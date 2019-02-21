#ifndef ZCHECKBOXLISTENER_H
#define ZCHECKBOXLISTENER_H
#include "zcheckbox.h"

class ZCheckbox; 

// Interface definition
class ZCheckboxListener {
public:
    ZCheckboxListener();
    virtual void onCheckChange(ZCheckbox* sender, bool isChecked);    
};

#endif
