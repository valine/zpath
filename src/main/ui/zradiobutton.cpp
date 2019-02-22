#include "ui/zradiobutton.h"


ZRadioButton::ZRadioButton(float maxWidth, float maxHeight, string resourcePath, vector<string> titles) :
	ZView(maxWidth, maxHeight) {

	mTitles = titles;

	mLabel = new ZLabel(maxWidth, 18, "roboto/Roboto-Medium.ttf", resourcePath);
	mLabel->setOffset(0, 0);
	mLabel->setText("Radio Button");
	mLabel->setGravity(ZView::topLeft);
	mLabel->setTextColor(vec3(1, 1, 1));
	addSubView(mLabel);

	for (uint i = 0; i < titles.size(); i++) {
		ZButton* button = new ZButton((getRight() - getLeft()) / titles.size(), maxHeight - 20, resourcePath);
	
		if (i == 0) {
		    button->setBackgroundColor(vec4(0.006512, 0.242281, 0.651406, 1));
		} else {
			button->setBackgroundColor(vec4(0.5));
		}
		
		button->setText(titles.at(i));
		button->setOnClickListener(this);
		button->setTag(titles.at(i));
		addSubView(button);
	}


	//ZLabel* label = new ZLabel(maxWidth, maxHeight, "roboto/Roboto-Regular.ttf", resourcePath);
	//label->setMargin(0,0,0,0);
	//addSubView(label);
}

void ZRadioButton::onClick(ZButton* sender) {
	int i = 0;
	for (vector<ZView*>::iterator it = getSubViews().begin() ; it != getSubViews().end(); ++it) {
        ZView* view = (*it);
        if (i > 0) {
	       view->setBackgroundColor(vec4(0.5));
    	} 

        i++;
    }

    sender->setBackgroundColor(vec4(0.006512, 0.242281, 0.651406, 1));

    setTag(sender->getText());

   	if (mListener != nullptr) {
   		mListener->onClick(sender);
   	}
}

void ZRadioButton::computeBounds(int windowHeight, int maxWidth) {

	for (uint i = 0; i < mTitles.size(); i++) {
		

		int width = getWidth() / mTitles.size();
		int height = getHeight() / mTitles.size();
		ZView *tile = getSubViews().at(i + 1);

		tile->setMaxWidth(width);
		tile->setMaxHeight(getHeight() - 18);

		tile->setOffset(i * width, 20);

	}

	ZView::computeBounds(windowHeight, maxWidth);
}

void ZRadioButton::draw() {
	ZView::draw();

}

void ZRadioButton::setText(string text) {
	mLabel->setText(text);
}

void ZRadioButton::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);
	int i = 0;
	for (vector<ZView*>::iterator it = getSubViews().begin() ; it != getSubViews().end(); ++it) {
        ZView* view = (*it);
        if (i > 0) {
	        bool isInViewX = view->getLeft() < x && view->getRight() > x;
	        bool isInViewY = view->getTop() < y && view->getBottom() > y;

	        if (isInViewY && isInViewX && action == GLFW_PRESS) {
	            view->onMouseEvent(button, action, mods, x, y);
	        }
    	}

        i++;
    }
}

void ZRadioButton::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);

}

void ZRadioButton::onCursorPosChange(double x, double y)  {
	// ZView::onCursorPosChange(x, y);

	// if (mouseIsDown()) {
	// 	int deltaX =  x - getMouseDownX();
	// 	int deltaY = y - getMouseDownY();
	// }
}

void ZRadioButton::setOnClickListener(ZOnClickListener* l) {
	mListener = l;
}