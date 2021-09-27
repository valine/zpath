//
// Created by lukas on 2/25/21.
//

#ifndef ZPATH_ZTEXTFIELD_H
#define ZPATH_ZTEXTFIELD_H


#include <functional>
#include "zlabel.h"

class ZTextField : public ZLabel {


public:

    enum FocusMode {
        singleClick,
        doubleClick
    };

    enum TextMode {
        field,
        editor
    };

    ZTextField(ZView *parent);

    void onCharacterInput(unsigned int code) override;
    void onMouseEvent(int button, int action, int mods, int sx, int sy) override;
    void onKeyPress(int key, int scancode, int action, int mods) override;
    void onGlobalMouseUp(int key) override;
    void onDoubleClick() override;
    void setText(string text) override;
    void setTextColor(vec3 color) override;

    void setOnReturn(function<void(string)> listener) {
        mOnReturn = listener;
    }

    void setOnTextChange(function<void(string)> listener) {
        mOnTextChange = listener;
    }

    void setInFocus();

    /**
     * Focus mode, 0 for click, 1 for double click;
     */
    void setFocusMode(FocusMode mode) {
        mFocusMode = mode;
    }

    void setTextMode(TextMode tm) {
        mTextMode = tm;
        updateTitle();
    }

    void setTitleText(string text) {
        mTitle->setText(text);
    }

    void setBackgroundColor(vec4 color) override;
private:

    ZLabel* mTitle = nullptr;
    ZView* mCursor = nullptr;
    int mCursorIndex = 0;
    int mLineIndex = 0;

    FocusMode mFocusMode = singleClick;
    TextMode mTextMode = editor;
    int mCursorOffset = 0;
    string mInitialText;
    bool mInFocus = false;
    function<void(string content)> mOnReturn;
    function<void(string)> mOnTextChange;

    string unicodeToStr(unsigned int codepoint) {
        std::string out;
        if (codepoint <= 0x7f) {
            out.append(1, static_cast<char>(codepoint));
        } else if (codepoint <= 0x7ff) {
            out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        } else if (codepoint <= 0xffff) {
            out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        } else {
            out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        }
        return out;
    }

    void cancelEdit();

    void applyEdit();

    void deleteCharacter();

    void onFocusChanged(ZView *viewWithFocus);

    void cursorToEnd();

    void cursorToPosition(int x, int y);

    void insertCharacter(string str);

    void updateCursorPosition();

    int getLineIndex();

    void updateCursorOffset();

    void startEdit();

    void updateTitle();

    void setVisibility(bool visible) override;
};


#endif //ZPATH_ZTEXTFIELD_H
