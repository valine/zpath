//
// Created by Lukas Valine on 6/23/21.
//

#ifndef ZPATH_ZDEVVIEWCONTROLLER_H
#define ZPATH_ZDEVVIEWCONTROLLER_H


#include "zviewcontroller.h"

class ZDevViewController : public ZViewController {

public:
    ZDevViewController(char **const pString);

    void onCreate() override;

};


#endif //ZPATH_ZDEVVIEWCONTROLLER_H
