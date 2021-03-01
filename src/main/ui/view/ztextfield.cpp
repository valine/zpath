#include <utility>

//
// Created by lukas on 2/25/21.
//

#include "ui/ztextfield.h"

ZTextField::ZTextField(ZView *parent)
        : ZLabel("Hellom", parent) {

    mCursor = new ZView(3, fillParent, this);
    mCursor->setBackgroundColor(black);
    mCursor->setVisibility(false);
    drawText();
    mCursor->setXOffset(getEndPoint().first);
}

void ZTextField::onCharacterInput(unsigned int code) {
    ZView::onCharacterInput(code);
    if (mInFocus) {
        string str = unicodeToStr(code);
        enterText(getText() + str);
    }
}

void ZTextField::onFocusChanged(ZView *viewWithFocus) {
    if (this != viewWithFocus) {
        applyEdit();
    }
}

void ZTextField::enterText(string str) {
    setText(std::move(str));
    drawText();
    int offset = getEndPoint().first;
    mCursor->setXOffset(offset);
    mCursor->onWindowChange(getWindowWidth(), getWindowHeight());
}

void ZTextField::deleteCharacter() {
    string text = getText();
    text.pop_back();
    enterText(text);
}

void ZTextField::setInFocus() {
    mInFocus = true;
    mInitialText = getText();
    mCursor->setVisibility(true);
    requestFocus(this);
}

void ZTextField::cancelEdit() {
    mInFocus = false;
    setText(mInitialText);
    mCursor->setVisibility(false);
    releaseFocus(this);
}

void ZTextField::applyEdit() {
    mInFocus = false;
    mCursor->setVisibility(false);
    releaseFocus(this);
}

void ZTextField::onKeyPress(int key, int scancode, int action, int mods) {
    ZView::onKeyPress(key, scancode, action, mods);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        cancelEdit();
    } else if (key == GLFW_KEY_ENTER) {
        applyEdit();
    } else if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        deleteCharacter();
    }
}

bool ZTextField::onMouseEvent(int button, int action, int mods, int sx, int sy) {
    bool consumed = ZView::onMouseEvent(button, action, mods, sx, sy);

    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
        setInFocus();
    }

    return consumed;
}


void ZTextField::onGlobalMouseUp(int key) {
    ZView::onGlobalMouseUp(key);
    if (!isMouseInBounds(this)) {
        applyEdit();
    }
}
