//
// Created by lukas on 2/28/21.
//

#ifndef ZPATH_ZTEXTVIEWCONTROLLER_H
#define ZPATH_ZTEXTVIEWCONTROLLER_H


#include <ui/zviewcontroller.h>

class ZTextViewController : public ZViewController {
public:
    ZTextViewController(char* argv[]) : ZViewController(argv) {}
    ZTextViewController(string path) : ZViewController(path) {}

private:
    void onCreate() override;

};


#endif //ZPATH_ZTEXTVIEWCONTROLLER_H
