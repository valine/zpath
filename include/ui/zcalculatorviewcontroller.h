//
// Created by Lukas Valine on 3/21/21.
//

#ifndef ZPATH_ZCALCULATORVIEWCONTROLLER_H
#define ZPATH_ZCALCULATORVIEWCONTROLLER_H

#include "zviewcontroller.h"

class ZCalculatorViewController : public ZViewController {


public:
    ZCalculatorViewController(char* argv[]) : ZViewController(argv) {}
    ZCalculatorViewController(string path) : ZViewController(path) {}

private:
    void onCreate() override;

};


#endif //ZPATH_ZCALCULATORVIEWCONTROLLER_H
