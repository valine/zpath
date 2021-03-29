#include "ui/zradiobutton.h"


ZRadioButton::ZRadioButton(float maxWidth, float maxHeight, string resourcePath, vector<string> titles) :
	ZView(maxWidth, maxHeight) {
    init(maxWidth, maxHeight, titles);
}

ZRadioButton::ZRadioButton(string label, vector<string> titles, ZView *parent) :
    ZView(10000, 45) {
    init(10000, 45, titles);
    setMargin(5,5,5,5);
    mLabel->setText(label);
    parent->addSubView(this);
}

ZRadioButton::ZRadioButton(float maxWidth, float maxHeight, vector<ZTexture *> backgrounds, vector<string> tags) :
	ZView(maxWidth, maxHeight) {

	mTitles = tags;
	mLabel = new ZLabel(maxWidth, 18);
	mLabel->setOffset(0, 0);
	mLabel->setText("");
	mLabel->setGravity(ZView::topLeft);
	mLabel->setTextColor(ZSettingsStore::getInstance().getHighlightTextColor());
	addSubView(mLabel);

	for (uint i = 0; i < tags.size(); i++) {
		auto* button = new ZButton((getRight() - getLeft()) / tags.size(), maxHeight - 20);
	
		button->setBackgroundImage(backgrounds.at(i));
		if (i == 0) {
		    button->setBackgroundColor(mHighlightColor);
		      mHighlighted = button;
            button->getLabel()->setTextColor(ZSettingsStore::get().getBaseTextColor());
		} else {
			button->setBackgroundColor(mBaseColor);
			button->getLabel()->setTextColor(ZSettingsStore::get().getBaseTextColor());
		}
		
		button->setText("");
		button->setTag(tags.at(i));
		button->setOnClickListener(this);
		mButtons.push_back(button);
		addSubView(button);
	}
}
void ZRadioButton::init(float maxWidth, float maxHeight, vector<string> &titles) {
    mTitles = titles;
    mLabel = new ZLabel(maxWidth, 18);
    mLabel->setOffset(0, 0);
    mLabel->setText("Radio Button");
    mLabel->setGravity(topLeft);
    mLabel->setTextColor(ZSettingsStore::getInstance().getBaseTextColor());
    addSubView(mLabel);

    for (uint i = 0; i < titles.size(); i++) {
        auto* button = new ZButton((getRight() - getLeft()) / titles.size(), maxHeight - 20);

        if (i == 0) {
            button->setBackgroundColor(mHighlightColor);
            mHighlighted = button;
            button->getLabel()->setTextColor(ZSettingsStore::get().getBaseTextColor());
        } else {
            button->setBackgroundColor(mBaseColor);
            button->getLabel()->setTextColor(ZSettingsStore::get().getBaseTextColor());
        }

        button->setText(titles.at(i));
        button->setOnClickListener(this);
        button->setTag(titles.at(i));
        mButtons.push_back(button);
        addSubView(button);
    }
}

void ZRadioButton::showLabel(bool shouldShow) {
	mLabel->setVisibility(shouldShow);
}

void ZRadioButton::setHighlightColor(vec4 color) {
	mHighlightColor = color;

	for (uint i = 0; i < getSubViews().size(); i++) {
        ZView* view = getSubViews().at(i);
        if (i > 0) {
	       view->setBackgroundColor(mBaseColor);
    	} 
    }

    if (mHighlighted != nullptr) {
    	mHighlighted->setBackgroundColor(mHighlightColor);
	}
}

void ZRadioButton::setBaseColor(vec4 color) {
	mBaseColor = color;
	for (uint i = 0; i < getSubViews().size(); i++) {
        ZView* view = getSubViews().at(i);
        if (i > 0) {
	       view->setBackgroundColor(mBaseColor);
    	} 
    }

    if (mHighlighted != nullptr) {
    	mHighlighted->setBackgroundColor(mHighlightColor);
	}
}

void ZRadioButton::onClick(ZButton* sender) {
//	ZOnClickListener::onClick(sender);
    int index = 0;
	for (uint i = 0; i < getSubViews().size(); i++) {
        ZView* view = getSubViews().at(i);
        if (i > 0) {
	       view->setBackgroundColor(mBaseColor);
	       if (view == sender) {
	           index = i - 1;
	       }
    	}
    }
    
    mHighlighted = sender;
    sender->setBackgroundColor(mHighlightColor);

    // cout << "radio" << endl;
   	if (mListener != nullptr) {
   	 	mListener->onClick(sender);
   	 }

   	if (mOnClick != nullptr) {
   	    mOnClick(this, index);
   	}
}

void ZRadioButton::setVertical(bool isVertical) {
	mIsVertical = isVertical;
}

void ZRadioButton::setSpacing(int spacing) {
	mSpacing = spacing;
}

void ZRadioButton::computeBounds() {
	ZView::computeBounds();
	int labelMargin = 20;

	if (!mLabel->getVisibility()) {
		labelMargin = 0;
	}

	if (mIsVertical) {
		for (uint i = 0; i < mTitles.size(); i++) {
			int width = getWidth() / mTitles.size();
			int height = ((getHeight() - labelMargin) / mTitles.size()) - mSpacing;
			ZView *tile = getSubViews().at(i + 1);

			tile->setMaxWidth(getWidth());
			tile->setMaxHeight(height);
			tile->setOffset(0, i * (height + mSpacing) + labelMargin);
		}
	} else {
		for (uint i = 0; i < mTitles.size(); i++) {
			int width = (getWidth() / mTitles.size()) - mSpacing;
			int height = getHeight() / mTitles.size();
			ZView *tile = getSubViews().at(i + 1);

			tile->setMaxWidth(width);
			tile->setMaxHeight(getHeight() - labelMargin);
			tile->setOffset(i * (width + mSpacing), labelMargin);

		}
	}


}

void ZRadioButton::draw() {
	ZView::draw();
}

void ZRadioButton::setText(string text) {
	mLabel->setText(text);
}

void ZRadioButton::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);
	for (int i = 0; i < getSubViews().size(); i++) {
        ZView* view = getSubViews().at(i);
        if (i > 0) {
	        if (  isMouseInBounds(view) && action == GLFW_PRESS) {
	            view->onMouseEvent(button, action, mods, x, y);
	        }
    	}
    }
}

void ZRadioButton::onKeyPress(int key, int scancode, int action, int mods) {

}

void ZRadioButton::onCursorPosChange(double x, double y)  {
	 ZView::onCursorPosChange(x, y);

	// if (mouseIsDown()) {
	// 	int deltaX =  x - getMouseDownX();
	// 	int deltaY = y - getMouseDownY();
	// }
}

void ZRadioButton::setOnClickListener(ZOnClickListener* l) {
	mListener = l;
}

ZButton *ZRadioButton::getButton(int index) {
    return mButtons.at(index);
}

void ZRadioButton::setOnClick(function<void(ZView *, int)> onclick) {
    mOnClick = onclick;
}

