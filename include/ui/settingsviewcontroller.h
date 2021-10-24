//
// Created by Lukas Valine on 10/24/21.
//

#ifndef ZPATH_SETTINGSVIEWCONTROLLER_H
#define ZPATH_SETTINGSVIEWCONTROLLER_H


#include "zviewcontroller.h"


class SettingsViewController : public ZViewController {


public:
    SettingsViewController(char* argv[]) : ZViewController(argv) {}
    SettingsViewController(string path) : ZViewController(path) {}

private:
    void onCreate() override;

};



#endif //ZPATH_SETTINGSVIEWCONTROLLER_H
