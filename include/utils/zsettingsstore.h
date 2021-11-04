//
// Created by lukas on 7/14/19.
//

#ifndef ZPATH_ZSETTINGSSTORE_H
#define ZPATH_ZSETTINGSSTORE_H

static const int LIGHT = 0;
static const int DARK = 1;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "ui/zcolor.h"

using namespace glm;
using namespace std;

class ZSettingsStore {

public:
    static ZSettingsStore& getInstance(){
        static ZSettingsStore instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    static ZSettingsStore& get(){
        return getInstance();
    }

    ZColor getHighlightColor();
    ZColor getBaseColor();
    ZColor getInactiveColor();
    ZColor getBackgroundColor();
    ZColor getHighlightTextColor();
    ZColor getBaseTextColor();
    string getResourcePath();
    void setResourcePath(string path);

    void setHighlightColor(ZColor color);
    void setBaseColor(ZColor color);
    void setInactiveColor(ZColor color);
    void setBackgroundColor(ZColor color);
    void setHighlightTextColor(ZColor color);
    void setBaseTextColor(ZColor color);

    void setColorMode(ColorMode mode);
    ColorMode getColorMode();

    void setOnThemeChange(function<void()> fn) {
        mOnThemeChange = fn;
    }

private:

    function<void()> mOnThemeChange = nullptr;
    ZColor mBaseColor = ZColor(vec4(0.9, 0.9, 0.92, 1.0),
                               vec4(0.01, 0.01, 0.011, 1.0) );
    ZColor mBackgroundColor = ZColor(vec4(0.85, 0.86, 0.88, 1.0),
                                     vec4(0.02, 0.022, 0.03, 1.0));
    ZColor mHighlightColor = vec4(0.036964, 0.260146, 0.679894, 1.000000);
    ZColor mInactiveColor = vec4(0.0,0.0,0.0,0.5);
    ZColor mHighlightTextColor = ZColor(vec4(1), vec4(0));
    ZColor mBaseTextColor = ZColor(vec4(0), vec4(1));

    string mResourcePath;
    ZSettingsStore();
    static ZSettingsStore *mInstance;
    ZSettingsStore(ZSettingsStore const&);              // Don't Implement
    void operator=(ZSettingsStore const&); // Don't implement

};


#endif //ZPATH_ZSETTINGSSTORE_H
