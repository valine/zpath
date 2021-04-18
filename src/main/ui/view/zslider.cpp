#include <functional>
#include <utils/zsettingsstore.h>
#include "ui/zslider.h"

ZSlider::ZSlider(float maxWidth, float maxHeight, string label): 
ZView(maxWidth, maxHeight) {
	float lineHeight = SLIDER_THUMB_SIZE;
	mLine = new ZView(getWidth(), lineHeight);
    mLine->setCornerRadius(SLIDER_THUMB_SIZE / 2);
	mLine->setOffset(0,getHeight() / 2 - (lineHeight / 2));
	mLine->setBackgroundColor(ZSettingsStore::getInstance().getInactiveColor());
	mLine->setGravity(ZView::bottomLeft);
	addSubView(mLine);

	mHighlight = new ZView(SLIDER_THUMB_SIZE, SLIDER_THUMB_SIZE);
    mHighlight->setCornerRadius(SLIDER_THUMB_SIZE / 2);
	setFillColor(ZSettingsStore::getInstance().getHighlightColor());
	mHighlight->setOffset(0,getHeight() / 2 - (SLIDER_THUMB_SIZE / 2));
	mHighlight->setGravity(ZView::bottomLeft);
	addSubView(mHighlight);

	mThumb = new ZView(SLIDER_THUMB_SIZE, SLIDER_THUMB_SIZE);
    mThumb->setCornerRadius(2);
	mThumb->setBackgroundColor(ZSettingsStore::getInstance().getBaseColor());
	mThumb->setOffset(0,getHeight() / 2 - (SLIDER_THUMB_SIZE / 2));
	mThumb->setGravity(ZView::bottomLeft);
	addSubView(mThumb);

	mTitle = label;
}

float ZSlider::getValue() {
	return mValue;
}

void ZSlider::setTextColor(vec3 color) {
	mLabel->setTextColor(color);
}

void ZSlider::setHighlightBackground(ZTexture* tex) {
	mHighlight->setBackgroundImage(tex);
}

void ZSlider::setThumbBackground(ZTexture* tex) {
	mThumb->setBackgroundImage(tex);
}

ZSlider::ZSlider(string label, float min, float max, float value, ZView *parent) :
ZView(ZView::fillParent, 40, parent){
    //setBackgroundColor(parent->getBackgroundColor());
    float lineHeight = SLIDER_THUMB_SIZE;
    mLine = new ZView(getWidth(), lineHeight);
    mLine->setCornerRadius(SLIDER_THUMB_SIZE / 2);
    mLine->setOffset(0, Y_OFFSET - (lineHeight / 2));
    mLine->setBackgroundColor(ZSettingsStore::getInstance().getInactiveColor());
    mLine->setGravity(ZView::bottomLeft);
    addSubView(mLine);

    float cr = SLIDER_THUMB_SIZE / 2;
    mHighlight = new ZView(SLIDER_THUMB_SIZE, SLIDER_THUMB_SIZE);
    mHighlight->setCornerRadius(vec4(cr, 0,0, cr));

    setFillColor(ZSettingsStore::getInstance().getHighlightColor());
    mHighlight->setOffset(0, Y_OFFSET - (SLIDER_THUMB_SIZE / 2));
    mHighlight->setGravity(ZView::bottomLeft);
    addSubView(mHighlight);

    mThumb = new ZView(SLIDER_THUMB_SIZE, SLIDER_THUMB_SIZE);
    mThumb->setCornerRadius(SLIDER_THUMB_SIZE / 2);
    mThumb->setLineWidth(0.0);
    mThumb->setOutlineColor(transparent);
    mThumb->setBackgroundColor(ZSettingsStore::getInstance().getBaseColor());
    mThumb->setOffset(0, Y_OFFSET - (SLIDER_THUMB_SIZE / 2));
    mThumb->setGravity(ZView::bottomLeft);
    addSubView(mThumb);

    mTitle = label;

    mLabel = new ZLabel(ZView::fillParent, 18);
    mLabel->setOffset(0,0);
    mLabel->setGravity(ZView::topLeft);
    addSubView(mLabel);

    setMargin(5,5,5,5);
    setMaxValue(max);
    setMinValue(min);

    setTextColor(ZSettingsStore::getInstance().getBaseTextColor());
    setValue(value);
}

ZSlider::ZSlider(float maxWidth, float maxHeight, string title, string resourcePath) : 
ZSlider(maxWidth, maxHeight, title) {
	
	mLabel = new ZLabel(maxWidth, 18);
    mLabel->setOffset(0,0);
    mLabel->setTextColor(vec4(1.0));
    mLabel->setGravity(ZView::topLeft);
    addSubView(mLabel);
    setValue(0.0);
}

void ZSlider::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);
 	if (action == GLFW_RELEASE) {

        float cr = (SLIDER_THUMB_SIZE / 2);

        float offset = mThumb->getOffsetX();
		float maxOffset = (float) (getWidth() - SLIDER_THUMB_SIZE);
		float factor = (float) offset / maxOffset;
		float value = ((mMaxValue - mMinValue) * factor) + mMinValue;
		float incValue = roundf(value / mIncrement) * mIncrement;
 
 		mHighlight->setMaxWidth(mThumb->getOffsetX() + 1 + cr);

 		mValue = incValue;
 		if (mListener != nullptr) {
			mListener->onSliderValueChanged(this, incValue);
		}

        if (mSlideListener != nullptr) {
            mSlideListener(this, incValue, true);
        }

        invalidate();
 	} else if (action == GLFW_PRESS) {
        mInitialOffset = getMouse().x;
        mInitialThumb = mThumb->getOffsetX();
 	}
}

void ZSlider::setIncrement(float increment) {
	mIncrement = increment;
}


void ZSlider::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);
}
	
void ZSlider::onCursorPosChange(double x, double y) {
	ZView::onCursorPosChange(x, y);

	float cr = (SLIDER_THUMB_SIZE / 2);
	float scale = getScale().x;
	if (mouseIsDown() && !shiftKeyPressed() && !altKeyPressed()) {
		float newOffset = mInitialThumb + (x - mInitialOffset);

		if (newOffset < 0) {
			newOffset = 0;
		} else if (newOffset > getWidth() - SLIDER_THUMB_SIZE) {
			newOffset = getWidth() - SLIDER_THUMB_SIZE;
		}

		int yPosition = Y_OFFSET - (SLIDER_THUMB_SIZE / 2);
		mThumb->setOffset(newOffset, yPosition);
		mHighlight->setMaxWidth(newOffset + 1 + cr);
		valueChanged(newOffset);
	} else if (mouseIsDown() && shiftKeyPressed() && !altKeyPressed()) {
        int deltaX = getMouseDragDelta().x;
        float newOffset = mInitialThumb + 0.1 * (deltaX - mInitialOffset);

		if (newOffset < 0) {
			newOffset = 0;
		} else if (newOffset > getWidth() - SLIDER_THUMB_SIZE) {
			newOffset = getWidth() - SLIDER_THUMB_SIZE;
		}

		int yPosition = Y_OFFSET - (SLIDER_THUMB_SIZE / 2);
		mThumb->setOffset(newOffset, yPosition);
		mHighlight->setMaxWidth(newOffset + 1 + cr);
		valueChanged(newOffset);
	} else if (mouseIsDown() && !shiftKeyPressed() && altKeyPressed()) {
        int deltaX = getMouseDragDelta().x;
        float newOffset = mInitialThumb + (deltaX - mInitialOffset);

        if (newOffset < 0) {
			newOffset = 0;
		} else if (newOffset > getWidth() - SLIDER_THUMB_SIZE) {
			newOffset = getWidth() - SLIDER_THUMB_SIZE;
		}

		int yPosition = Y_OFFSET - (SLIDER_THUMB_SIZE / 2);
		mThumb->setOffset(newOffset, yPosition);
		valueChanged(newOffset);
	} else if (mouseIsDown() && shiftKeyPressed() && altKeyPressed()) {
        int deltaX = getMouseDragDelta().x;
        float newOffset = mInitialThumb + 0.1 * (deltaX - mInitialOffset);


		if (newOffset < 0) {
			newOffset = 0;
		} else if (newOffset > getWidth() - SLIDER_THUMB_SIZE) {
			newOffset = getWidth() - SLIDER_THUMB_SIZE;
		}

		int yPosition = Y_OFFSET - (SLIDER_THUMB_SIZE / 2);
		mThumb->setOffset(newOffset, yPosition);
		valueChanged(newOffset);
	}
}

void ZSlider::onScrollChange(double x, double y) {
	ZView::onScrollChange(x, y);

	int currentOffset = mThumb->getOffsetX();

	int newOffset;

	if (altKeyPressed()) {
		newOffset = currentOffset + (getWidth() / 40 * y);
	} else {
		newOffset = currentOffset + (getWidth() / 10 * y);
	}

	if (newOffset < 0) {
		newOffset = 0;
	} else if (newOffset > getWidth() - SLIDER_THUMB_SIZE) {
		newOffset = getWidth() - SLIDER_THUMB_SIZE;
	}

    float cr = (SLIDER_THUMB_SIZE / 2);
	int yPosition = Y_OFFSET - (SLIDER_THUMB_SIZE / 2);
	mThumb->setOffset(newOffset, yPosition);
	mHighlight->setMaxWidth(newOffset + 1 + cr);
	valueChanged(newOffset);
}

void ZSlider::setListener(ZSliderListener* listener) {
	mListener = listener;
}

void ZSlider::valueChanged(float offset) {
    float cr = (SLIDER_THUMB_SIZE / 2);
	float maxOffset = (float) (getWidth() - SLIDER_THUMB_SIZE);
	float factor = (float) offset / maxOffset;
	float value = ((mMaxValue - mMinValue) * factor) + mMinValue;
	
	float incValue = roundf(value / mIncrement) * mIncrement;
	auto format = "%.4f";
	auto size = std::snprintf(nullptr, 0, format, incValue);
	std::string output(size, '\0');
	std::sprintf(&output[0], format, incValue);

	mValue = incValue;
	if (mLabel != nullptr && mShowLabel) {
        mLabel->setText(mTitle + " " + output);
    }
	if (mListener != nullptr && !altKeyPressed()) {
		mHighlight->setMaxWidth(offset + 1 + cr);
		mListener->onSliderValueChanged(this, incValue);

	}

	if (mSlideListener != nullptr) {
        mHighlight->setMaxWidth(offset + 1 + cr);
        mSlideListener(this, incValue, false);
	}

	invalidate();
}

void ZSlider::setMaxValue(float max) {
	mMaxValue = max;
}

void ZSlider::setMinValue(float min) {
	mMinValue = min;
}

void ZSlider::setValue(float value) {
    float cr = (SLIDER_THUMB_SIZE / 2);

    float maxOffset = (float) (getWidth() - SLIDER_THUMB_SIZE);
	float factor = (value - mMinValue) / (mMaxValue - mMinValue);
	float offset = factor * maxOffset;
	int yPosition = Y_OFFSET - (SLIDER_THUMB_SIZE / 2);

	float newOffset= offset;
	if (newOffset < 0) {
		newOffset = 0;
	} else if (newOffset > getWidth() - SLIDER_THUMB_SIZE) {
		newOffset = getWidth() - SLIDER_THUMB_SIZE;
	}

	mThumb->setOffset(newOffset, yPosition);
	mHighlight->setMaxWidth(newOffset + cr);
	valueChanged(newOffset);
}

void ZSlider::setLineColor(vec4 color) {
	mLine->setBackgroundColor(color);
}


void ZSlider::setThumbColor(vec4 color) {
	mThumb->setBackgroundColor(color);
}

void ZSlider::setFillColor(vec4 color) {
    mHighlight->setBackgroundColor(color);
}

void ZSlider::setOnSlide(std::function<void(ZView*, float, bool)> onSlide) {
    mSlideListener = onSlide;
}

void ZSlider::setShowLabel(bool showLabel) {
    mShowLabel = showLabel;
    mLabel->setVisibility(showLabel);
}


