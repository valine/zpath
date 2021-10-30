//
// Created by Lukas Valine on 10/30/21.
//

#ifndef ZPATH_ZCOLOR_H
#define ZPATH_ZCOLOR_H

#include <glm/glm.hpp>
#include "utils/colormode.h"

using namespace glm;

class ZColor {
public:
    vec4 light;
    vec4 dark;

    ZColor(vec4 l, vec4 d) {
        light = l;
        dark = d;
    }

    ZColor(vec4 color) {
        light = color;
        dark = color;
    }

    ZColor operator=(vec4 color) {
        return {color, color};
    }

    ZColor operator=(vec3 color) {
        return {vec4(color,1.0), vec4(color, 1.0)};
    }

    ZColor operator+(ZColor color) const {
        return {light + color.light, dark + color.dark};

    }

    ZColor operator-(ZColor color) const {
        return {light - color.light, dark - color.dark};
    }


    ZColor operator+(vec4 color) const {
        return {light + color, dark + color};
    }

    ZColor operator-(vec4 color) const {
        return {light - color, dark - color};
    }

    vec4 get(ColorMode colorMode) {
        if (colorMode == ColorMode::light) {
            return light;
        } else {
            return dark;
        }
    }



   // vec4 faded = vec4(0.5, 0.5, 0.5, 0.2);
};



#endif //ZPATH_ZCOLOR_H
