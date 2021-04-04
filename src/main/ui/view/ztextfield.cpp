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

  //  setCornerRadius(10);
}

void ZTextField::cursorToEnd() {
    pair<int, int> offset = getEndPoint();
    mCursor->setXOffset(offset.first);
    mCursor->setYOffset(offset.second);
}

void ZTextField::cursorToPosition(int x, int y) {
    int yIndex = std::max(0, std::min(y / getLineHeight(), (int) getLineIndices().size() - 1));
    if (getLineIndices().empty()) {
        mCursorIndex = 0;
    } else {
        mCursorIndex = getLineIndices().at(yIndex) - 1;
    }

    updateCursorPosition();
    updateCursorOffset();
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
    updateCursorOffset();

    if (mOnTextChange != nullptr) {
        mOnTextChange(getText());
    }
}

void ZTextField::updateCursorPosition() {
    pair<int, int> offset = getPoints().at(std::max(0, mCursorIndex));
    mCursor->setXOffset(offset.first);
    mCursor->setYOffset(offset.second);
    mCursor->onWindowChange(getWindowWidth(), getWindowHeight());
}

int ZTextField::getLineIndex() {
    pair<int, int> offset = getPoints().at(std::max(0, mCursorIndex));
    return (int) std::max(0, (offset.second / getLineHeight()));
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

            if (mOnTextChange != nullptr) {
                mOnTextChange(getText());
            }
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
        applyEdit();
    } else if (key == GLFW_KEY_ENTER && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (mOnReturn == nullptr) {
            insertCharacter("\n");
        } else {
            mOnReturn(getText());
            applyEdit();
        }

    } else if (key == GLFW_KEY_TAB && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        //insertCharacter("\t");

        // Could pull from a setting
        int spaceCount = 5;
        for (int i = 0; i < spaceCount; i++) {
            insertCharacter(" ");
        }
    } else if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        deleteCharacter();
        updateCursorOffset();
    } else if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (mCursorIndex > 0) {
            mCursorIndex--;
            updateCursorOffset();
            updateCursorPosition();
        }
    } else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (mCursorIndex < getPoints().size() - 1) {
            mCursorIndex++;
            updateCursorOffset();
            updateCursorPosition();
        }
    } else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (mOnReturn == nullptr) {
            int currentLine = getLineIndex();
            if (currentLine > 0) {
                int lineIndex = getLineIndices().at(currentLine - 1);
                int previousLineIndex = 0;
                if (currentLine > 1) {
                    previousLineIndex = std::max(0, getLineIndices().at(currentLine - 2));
                }
                mCursorIndex = std::min(lineIndex - 1, previousLineIndex + mCursorOffset);
                updateCursorPosition();
            }
        }
    } else if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (mOnReturn == nullptr) {
            int currentLine = getLineIndex();
            if (currentLine < getLineIndices().size() - 1) {
                int nextLineIndex = getLineIndices().size() - 1;
                if (currentLine < getLineIndices().size()) {
                    nextLineIndex = std::max(0, getLineIndices().at(currentLine));
                }
                mCursorIndex = std::min(getLineIndices().at(currentLine + 1) - 1, nextLineIndex + mCursorOffset);
                updateCursorPosition();
            }
        }
    }
}

void ZTextField::updateCursorOffset() {
    if (!getLineIndices().empty()) {

        if (getLineIndex() != 0) {
            int currentLine = std::max(1, getLineIndex());
            int lineIndex = getLineIndices().at(currentLine - 1);
            mCursorOffset = mCursorIndex - lineIndex;
        } else {
            mCursorOffset = mCursorIndex;
        }
    } else {
        mCursorOffset = 0;
    }
}

void ZTextField::onMouseEvent(int button, int action, int mods, int sx, int sy) {
    ZView::onMouseEvent(button, action, mods, sx, sy);

    sx -= getLeft();
    sy -= getTop();

    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
        setInFocus();
        cursorToPosition(sx, sy);
    }

}

void ZTextField::onGlobalMouseUp(int key) {
    ZView::onGlobalMouseUp(key);
    if (!isMouseInBounds(this)) {
        applyEdit();
    }
}
