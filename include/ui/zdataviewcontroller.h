//
// Created by lukas on 1/28/22.
//

#ifndef ZPATH_ZDATAVIEWCONTROLLER_H
#define ZPATH_ZDATAVIEWCONTROLLER_H


#include "zviewcontroller.h"

class ZDataViewController : public ZViewController{
public:
    ZDataViewController(char **const pString) : ZViewController(pString) {}

private:
    void onCreate() override;
};


#endif //ZPATH_ZDATAVIEWCONTROLLER_H
