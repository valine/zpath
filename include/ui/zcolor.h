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
        return {max(light - color.light, vec4(0)),
                max(dark - color.dark, vec4(0))};
    }


    ZColor operator+(vec4 color) const {
        return {light + color, dark + color};
    }

    ZColor operator-(vec4 color) const {
        return {max(light - color, vec4(0)),
                max(dark - color, vec4(0))};
    }

    vec4 get(ColorMode colorMode) {
        if (colorMode == ColorMode::light) {
            return light;
        } else {
            return dark;
        }
    }

    /**
     * Returns a suitable text color given the background value
     * @return
     */
    ZColor getTextColor() {
        vec4 colorLight = get(ColorMode::light);
        vec4 colorDark = get(ColorMode::dark);
        ZColor textColor = ZColor(vec4(0));

        vec3 color3L = vec3(colorLight.r,colorLight.g,colorLight.b);
        vec3 color3D = vec3(colorDark.r,colorDark.g,colorDark.b);
        if (glm::length(color3L) < 0.8) {
            textColor.light = vec4(1);
        } else {
            textColor.light = vec4(0);
        }

        if (glm::length(color3D) < 0.8) {
            textColor.dark = vec4(1);
        } else {
            textColor.dark = vec4(0);
        }
        return textColor;
    }

    /**
    * Returns a suitable text color given the background value
    * @return
    */
    ZColor getHighlightColor(ZColor highlight) {
        vec4 colorLight = get(ColorMode::light);
        vec4 colorDark = get(ColorMode::dark);
        ZColor textColor = ZColor(vec4(0));

        vec3 color3L = vec3(colorLight.r,colorLight.g,colorLight.b);
        vec3 color3D = vec3(colorDark.r,colorDark.g,colorDark.b);
        if (glm::length(color3L) < 0.8 && colorLight.a != 0) {
            textColor.light = highlight.light;
        } else {
            textColor.light = vec4(0);
        }

        if (glm::length(color3D) < 0.8 && colorDark.a != 0) {
            textColor.dark = highlight.dark;
        } else {
            textColor.dark = vec4(0);
        }
        return textColor;
    }

    // vec4 faded = vec4(0.5, 0.5, 0.5, 0.2);
};



#endif //ZPATH_ZCOLOR_H
