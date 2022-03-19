//
// Created by lukas on 3/19/22.
//

#ifndef ZPATH_ZJSONEDITORVIEWCONTROLLER_H
#define ZPATH_ZJSONEDITORVIEWCONTROLLER_H

#include <ui/zviewcontroller.h>


class ZJsonEditorViewController : public ZViewController {

public:
    ZJsonEditorViewController(char* argv[]) : ZViewController(argv) {}
    ZJsonEditorViewController(string path) : ZViewController(path) {}

private:
    void onCreate() override;
};


#endif //ZPATH_ZJSONEDITORVIEWCONTROLLER_H
