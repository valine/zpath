//
// Created by lukas on 11/28/20.
//

#ifndef ZPATH_ZMAGNITUDEPICKER_H
#define ZPATH_ZMAGNITUDEPICKER_H


#include <ui/zview.h>


class ZMagnitudePicker : public ZView {
public:

    ZMagnitudePicker(ZView *parent);

private:
    void onGlobalMouseUp() override;

    void onMouseEvent(int button, int action, int mods, int sx, int sy);
};


#endif //ZPATH_ZMAGNITUDEPICKER_H
