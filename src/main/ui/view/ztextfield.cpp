//
// Created by lukas on 2/25/21.
//

#include "ui/ztextfield.h"

ZTextField::ZTextField(ZView *parent)
        : ZLabel("Hellom", parent) {

}

void ZTextField::onCharacterInput(unsigned int code) {
    ZView::onCharacterInput(code);
    if (mInFocus) {
        setText(getText() + unicodeToStr(code));
    }
}

bool ZTextField::onMouseEvent(int button, int action, int mods, int sx, int sy) {
    bool consumed = ZView::onMouseEvent(button, action, mods, sx, sy);

    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
        setInFocus();
    }

    return consumed;
}

void ZTextField::setInFocus() {
    mInFocus = true;
    setTextColor(red);
    mInitialText = getText();
}

void ZTextField::cancelEdit() {
    mInFocus = false;
    setText(mInitialText);
    setTextColor(black);
}

void ZTextField::applyEdit() {
    mInFocus = false;
    setTextColor(black);
}

void ZTextField::onKeyPress(int key, int scancode, int action, int mods) {
    ZView::onKeyPress(key, scancode, action, mods);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        cancelEdit();
    } else if (key == GLFW_KEY_ENTER) {
        applyEdit();
    }
}

void ZTextField::onGlobalMouseUp(int key) {
    ZView::onGlobalMouseUp(key);
    if (!isMouseInBounds(this)) {
        applyEdit();
    }
}


