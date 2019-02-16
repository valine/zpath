#include "ui/zslider.h"

ZSlider::ZSlider(float maxWidth, float maxHeight, string label): 
ZView(maxWidth, maxHeight) {
	
	float lineColor[4] = {0.0, 0.0, 0.0, 1.0};
	float thumbColor[4] = {0.1, 0.2, 0.9, 1.0};

	float lineHeight = SLIDER_THUMB_SIZE;
	mLine = new ZView(getWidth(), lineHeight);
	mLine->setOffset(0,getHeight() / 2 - (lineHeight / 2));
	mLine->setBackgroundColor(lineColor);
	mLine->setGravity(ZView::bottomLeft);
	addSubView(mLine);

	mHighlight = new ZView(SLIDER_THUMB_SIZE, SLIDER_THUMB_SIZE);
	mHighlight->setBackgroundColor(vec4(0.046663, 0.213823, 0.384374, 1.000000));
	mHighlight->setOffset(0,getHeight() / 2 - (SLIDER_THUMB_SIZE / 2));
	mHighlight->setGravity(ZView::bottomLeft);
	addSubView(mHighlight);

	mThumb = new ZView(SLIDER_THUMB_SIZE, SLIDER_THUMB_SIZE);
	mThumb->setBackgroundColor(thumbColor);
	mThumb->setOffset(0,getHeight() / 2 - (SLIDER_THUMB_SIZE / 2));
	mThumb->setGravity(ZView::bottomLeft);
	addSubView(mThumb);

	mTitle = label;
}


ZSlider::ZSlider(float maxWidth, float maxHeight, string title, string resourcePath) : 
ZSlider(maxWidth, maxHeight, title) {
	
	mLabel = new ZLabel(maxWidth, 18, "roboto/Roboto-Medium.ttf", resourcePath);
    mLabel->setOffset(0,0);
    mLabel->setTextColor(vec4(1.0));
    mLabel->setGravity(ZView::topLeft);
    addSubView(mLabel);
    setValue(0.0);
}


void ZSlider::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);
 	
 	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {

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

	if (mouseIsDown() && !shiftKeyPressed()) {

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
		mHighlight->setMaxWidth(newOffset);
		valueChanged(newOffset);
	} else if (mouseIsDown() && shiftKeyPressed()) {
		double deltaX =  (x - getLastX());
		double currentOffset = mThumb->getOffsetX();
		double newOffset = currentOffset + (deltaX * 0.1);

		if (newOffset < 0) {
			newOffset = 0;
		} else if (newOffset > getWidth() - SLIDER_THUMB_SIZE) {
			newOffset = getWidth() - SLIDER_THUMB_SIZE;
		}

		int yPosition = getHeight() / 2 - (SLIDER_THUMB_SIZE / 2);
		mThumb->setOffset(newOffset, yPosition);
		mHighlight->setMaxWidth(newOffset);
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
	mHighlight->setMaxWidth(newOffset);
	valueChanged(newOffset);
}

void ZSlider::setListener(ZSliderListener* listener) {
	mListener = listener;
}

void ZSlider::valueChanged(float offset) {
	float maxOffset = (float) (getWidth() - SLIDER_THUMB_SIZE);
	float factor = (float) offset / maxOffset;
	float value = ((mMaxValue - mMinValue) * factor) + mMinValue;
	
	float incValue = roundf(value / mIncrement) * mIncrement;

	auto format = "%.2f";
	auto size = std::snprintf(nullptr, 0, format, incValue);
	std::string output(size + 1, '\0');
	std::sprintf(&output[0], format, incValue);

	mLabel->setText(mTitle + " " + output);
	if (mListener != nullptr) {
		mListener->onSliderValueChanged(this, incValue);
	}
}

void ZSlider::setMaxValue(float max) {
	mMaxValue = max;
}

void ZSlider::setMinValue(float min) {
	mMinValue = min;
}

void ZSlider::setValue(float value) {
	float maxOffset = (float) (getWidth() - SLIDER_THUMB_SIZE);
	float factor = (value - mMinValue) / (mMaxValue - mMinValue);
	float offset = factor * maxOffset;
	int yPosition = getHeight() / 2 - (SLIDER_THUMB_SIZE / 2);

	float newOffset= offset;
	if (newOffset < 0) {
		newOffset = 0;
	} else if (newOffset > getWidth() - SLIDER_THUMB_SIZE) {
		newOffset = getWidth() - SLIDER_THUMB_SIZE;
	}

	mThumb->setOffset(newOffset, yPosition);
	mHighlight->setMaxWidth(newOffset);
	valueChanged(newOffset);
}

void ZSlider::setLineColor(vec4 color) {
	mLine->setBackgroundColor(color);
}


void ZSlider::setThumbColor(vec4 color) {
	mThumb->setBackgroundColor(color);
}


