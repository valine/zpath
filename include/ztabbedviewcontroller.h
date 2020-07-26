//
// Created by lukas on 7/26/20.
//

#ifndef ZPATH_ZTABBEDVIEWCONTROLLER_H
#define ZPATH_ZTABBEDVIEWCONTROLLER_H


#include <ui/zviewcontroller.h>

class ZTabbedViewController : public ZViewController {

public:
    ZTabbedViewController(char* argv[], vector<ZViewController*> controllers);
    ZTabbedViewController(string path, vector<ZViewController*> controllers);

private:
    vector<ZViewController*> mControllers;
    void onCreate();

};


#endif //ZPATH_ZTABBEDVIEWCONTROLLER_H
