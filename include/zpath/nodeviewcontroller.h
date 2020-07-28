//
// Created by lukas on 7/27/20.
//

#ifndef ZPATH_NODEVIEWCONTROLLER_H
#define ZPATH_NODEVIEWCONTROLLER_H


#include <ui/zviewcontroller.h>

class NodeViewController : public ZViewController {

public:
    NodeViewController(char* argv[]) : ZViewController(argv) {}
    NodeViewController(string path) : ZViewController(path) {}

private:
    void onCreate() override;
};

#endif //ZPATH_NODEVIEWCONTROLLER_H
