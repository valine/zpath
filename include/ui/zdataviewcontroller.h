//
// Created by lukas on 1/28/22.
//

#ifndef ZPATH_ZDATAVIEWCONTROLLER_H
#define ZPATH_ZDATAVIEWCONTROLLER_H


#include "zviewcontroller.h"
#include "ui/zlistview.h"
class ZDataViewController : public ZViewController{
public:
    ZDataViewController(char **const pString) : ZViewController(pString) {}

private:
    void onCreate() override;

    void onFileDrop(int count, const char **paths) override;
    ZListView* mListView;
    string getFileExtension(string path);

    void loadDataFile(string path);
};


#endif //ZPATH_ZDATAVIEWCONTROLLER_H
