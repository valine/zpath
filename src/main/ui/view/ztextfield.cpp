#include <utility>
#include <utils/zsettingsstore.h>

//
// Created by lukas on 2/25/21.
//

#include "ui/ztextfield.h"

ZTextField::ZTextField(ZView *parent)
        : ZLabel("", parent) {

    mCursor = new ZView(2, 12, this);
    mCursor->setBackgroundColor(ZSettingsStore::get().getHighlightColor());
    mCursor->setVisibility(false);
    drawText();

    cursorToEnd();
    setBackgroundColor(grey);

    setYOffset(120);
}

void ZTextField::cursorToEnd() {
    pair<int, int> offset = getEndPoint();
    mCursor->setXOffset(offset.first);
    mCursor->setYOffset(offset.second);
}

void ZTextField::cursorToPosition(int x, int y) {

    cout << "x" << x << "y" << y << endl;
    int yIndex = std::max(0, std::min(y / getLineHeight(), (int) getLineIndices().size() - 1));

    cout << "index" << yIndex << endl;
    if (getLineIndices().empty()) {
        mCursorIndex = 0;
    } else {
        mCursorIndex = getLineIndices().at(yIndex) - 1;
    }

    updateCursorPosition();
}

void ZTextField::onCharacterInput(unsigned int code) {
    ZView::onCharacterInput(code);
    if (mInFocus) {
        string str = unicodeToStr(code);
        insertCharacter(str);
    }
}

void ZTextField::insertCharacter(string str) {
    setText(std::move(getText().insert(mCursorIndex, str)));
    drawText();
    mCursorIndex++;
    updateCursorPosition();
}

void ZTextField::updateCursorPosition() {
    pair<int, int> offset = getPoints().at(std::max(0, mCursorIndex));
    mCursor->setXOffset(offset.first);
    mCursor->setYOffset(offset.second);
    mCursor->onWindowChange(getWindowWidth(), getWindowHeight());
}

void ZTextField::onFocusChanged(ZView *viewWithFocus) {
    if (this != viewWithFocus) {
        applyEdit();
    }
}

void ZTextField::deleteCharacter() {
    string text = getText();
    if (!text.empty()) {
        if (mCursorIndex > 0) {
            mCursorIndex-=1;
            text.erase(mCursorIndex, 1);
            setText(std::move(text));
            drawText();
            updateCursorPosition();
        }
    }
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
    } else if (key == GLFW_KEY_ENTER && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        insertCharacter("\n");
    } else if (key == GLFW_KEY_TAB && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //insertCharacter("\t");

        // Could pull from a setting
        int spaceCount = 5;
        for (int i = 0; i < spaceCount; i++) {
            insertCharacter(" ");
        }
    }else if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        deleteCharacter();
    }
}

bool ZTextField::onMouseEvent(int button, int action, int mods, int sx, int sy) {
    bool consumed = ZView::onMouseEvent(button, action, mods, sx, sy);

    sx -= getLeft();
    sy -= getTop();

    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
        setInFocus();
        cursorToPosition(sx, sy);
    }

    return consumed;
}


void ZTextField::onGlobalMouseUp(int key) {
    ZView::onGlobalMouseUp(key);
    if (!isMouseInBounds(this)) {
        applyEdit();
    }
}
