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

    void setValueChangedListener(function<void(int index, float value, bool isInput, int magIndex)> l);
    void setShowAbove(bool above);
    void setValue(float value);
    void setTitle(string name);
    void setInputType(int index, bool isInput) {
        mIsInput = isInput;
        mSocketIndex = index;
    }

    void selectMagnitude(int index);

    void setSliderVisibility(bool visibility);

    void setRadioButtonNames(vector<string> names);

private:
    void onGlobalMouseUp(int key) override;

    void onMouseEvent(int button, int action, int mods, int sx, int sy);
    void onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) override;
    void onCursorPosChange(double x, double y) override;
    bool mSliderActivated = false;
    function<void(int index, float value, bool isInput, int magIndex)> mListener;
    ZSlider* mSlider;

    vector<string> mRadioButtonNames;
    vector<float> mRanges = {1e-6, 1e-5, 1e-4, 1e-3, 0.01, 0.1, 1, 10, 100, 1e3, 1e4, 1e5, 1e6};

    ZView* mRangeContainer;
    vector<ZLabel*> mRadioButtonLabels;
    ZLabel* mName;

    bool mIsInput = false;
    int mSocketIndex = 0;
    int mSelectedMagnitude = 6;
    bool mSliderClicked = false;

    void initializeRadioButtons();

};


#endif //ZPATH_ZMAGNITUDEPICKER_H
