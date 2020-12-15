//
// Created by lukas on 11/28/20.
//

#ifndef ZPATH_ZMAGNITUDEPICKER_H
#define ZPATH_ZMAGNITUDEPICKER_H


#include <ui/zview.h>
#include "zslider.h"


class ZMagnitudePicker : public ZView {
public:

    ZMagnitudePicker(ZView *parent);

    void setValueChangedListener(function<void(float value)> l);

private:
    void onGlobalMouseUp() override;

    bool onMouseEvent(int button, int action, int mods, int sx, int sy);
    void onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state) override;
    void onCursorPosChange(double x, double y) override;
    bool mSliderActivated = false;
    function<void(float value)> mListener;
    ZSlider* mSlider;
};


#endif //ZPATH_ZMAGNITUDEPICKER_H
