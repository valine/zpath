//
// Created by lukas on 11/28/20.
//

#ifndef ZPATH_ZMAGNITUDEPICKER_H
#define ZPATH_ZMAGNITUDEPICKER_H


static const int MAG_WIDTH = 300;

#include <ui/zview.h>
#include "zslider.h"


class ZMagnitudePicker : public ZView {
public:

    ZMagnitudePicker(ZView *parent);

    void setValueChangedListener(function<void(float value)> l);
    void setShowAbove(bool above);
private:
    void onGlobalMouseUp(int key) override;

    bool onMouseEvent(int button, int action, int mods, int sx, int sy);
    void onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) override;
    void onCursorPosChange(double x, double y) override;
    bool mSliderActivated = false;
    function<void(float value)> mListener;
    ZSlider* mSlider;

    vector<string> mRangeStrings = {"e-6", "e-5", "e-4", "e-3", "0.01", "0.1", "1", "10", "100", "e3", "e4", "e5", "e6"};
    vector<float> mRanges = {1e-6, 1e-5, 1e-4, 1e-3, 0.01, 0.1, 1, 10, 100, 1e3, 1e4, 1e5, 1e6};

    ZView* mRangeContainer;
    vector<ZLabel*> mRangeLabels;
};


#endif //ZPATH_ZMAGNITUDEPICKER_H
