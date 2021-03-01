//
// Created by lukas on 2/25/21.
//

#ifndef ZPATH_ZTEXTFIELD_H
#define ZPATH_ZTEXTFIELD_H


#include "zlabel.h"

class ZTextField : public ZLabel {


public:
    ZTextField(ZView *parent);

    void onCharacterInput(unsigned int code) override;
    bool onMouseEvent(int button, int action, int mods, int sx, int sy) override;
    void onKeyPress(int key, int scancode, int action, int mods) override;
    void onGlobalMouseUp(int key) override;

private:

    ZView* mCursor;

    string mInitialText;
    bool mInFocus = false;

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


    void setInFocus();

    void cancelEdit();

    void applyEdit();

    void enterText(string str);

    void deleteCharacter();

    void onFocusChanged(ZView *viewWithFocus);

    void cursorToEnd();
};


#endif //ZPATH_ZTEXTFIELD_H
