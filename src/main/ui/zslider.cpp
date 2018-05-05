#include "zslider.h"

ZSlider::ZSlider(float maxWidth, float maxHeight, string title): 
ZView(maxWidth, maxHeight) {
	
	float lineColor[4] = {0.0, 0.0, 0.0, 1.0};
	float thumbColor[4] = {0.1, 0.2, 0.9, 1.0};

	float lineHeight = 2;
	mLine = new ZView(getWidth(), lineHeight);
	mLine->setOffset(0,getHeight() / 2 - (lineHeight / 2));
	mLine->setBackgroundColor(lineColor);
	addSubView(mLine);

	mThumb = new ZView(SLIDER_THUMB_SIZE, SLIDER_THUMB_SIZE);
	mThumb->setBackgroundColor(thumbColor);
	mThumb->setOffset(0,getHeight() / 2 - (SLIDER_THUMB_SIZE / 2));
	addSubView(mThumb);
}

void ZSlider::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);
 	
 	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {

 	}
}

void ZSlider::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);
}
	
void ZSlider::onCursorPosChange(double x, double y) {
	ZView::onCursorPosChange(x, y);

	if (mouseIsDown()) {

		int deltaX =  getLastX() - x;
		int currentOffset = mThumb->getOffsetX();
		int newOffset = currentOffset - deltaX;

		if (newOffset < 0) {
			newOffset = 0;
		} else if (newOffset > getWidth() - SLIDER_THUMB_SIZE) {
			newOffset = getWidth() - SLIDER_THUMB_SIZE;
		}

		int yPosition = getHeight() / 2 - (SLIDER_THUMB_SIZE / 2);
		mThumb->setOffset(newOffset, yPosition);
		valueChanged(newOffset);
	}
}

void ZSlider::onScrollChange(double x, double y) {
	ZView::onScrollChange(x, y);

	int currentOffset = mThumb->getOffsetX();
	int newOffset = currentOffset + (getWidth() / 10 * y);

	if (newOffset < 0) {
		newOffset = 0;
	} else if (newOffset > getWidth() - SLIDER_THUMB_SIZE) {
		newOffset = getWidth() - SLIDER_THUMB_SIZE;
	}

	int yPosition = getHeight() / 2 - (SLIDER_THUMB_SIZE / 2);
	mThumb->setOffset(newOffset, yPosition);
	valueChanged(newOffset);
}

void ZSlider::setListener(ZSliderListener* listener) {
	mListener = listener;
}

void ZSlider::valueChanged(int offset) {
	float maxOffset = (float) (getWidth() - SLIDER_THUMB_SIZE);
	float factor = (float) offset / maxOffset;

	float value = ((mMaxValue - mMinValue) * factor) + mMinValue;

	if (mListener != nullptr) {
		mListener->onSliderValueChanged(this, value);
	}
}

void ZSlider::setMaxValue(float max) {
	mMaxValue = max;
}

void ZSlider::setMinValue(float min) {
	mMinValue = min;
}

void ZSlider::setValue(float value) {
	int offset = (mMinValue - value) / (-mMaxValue + mMinValue);
	int yPosition = getHeight() / 2 - (SLIDER_THUMB_SIZE / 2);
	mThumb->setOffset(offset, yPosition);

}


