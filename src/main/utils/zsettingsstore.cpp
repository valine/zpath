//
// Created by lukas on 7/14/19.
//

#include <iostream>
#include "utils/zsettingsstore.h"
using namespace std;

ZSettingsStore::ZSettingsStore() {

}

vec4 ZSettingsStore::getHighlightColor() {
    return mHighlightColor;
}
vec4 ZSettingsStore::getBaseColor() {
    return mBaseColor;
}

void ZSettingsStore::setBaseColor(vec4 color) {
    mBaseColor = color;
}

void ZSettingsStore::setHighlightColor(vec4 color) {
    mHighlightColor = color;
}

vec4 ZSettingsStore::getInactiveColor() {
    return mInactiveColor;
}

void ZSettingsStore::setInactiveColor(vec4 color) {
    mInactiveColor = color;
}

vec4 ZSettingsStore::getBackgroundColor() {
    return mBackgroundColor;
}

void ZSettingsStore::setBackgroundColor(vec4 color) {
    mBackgroundColor = color;
}

vec3 ZSettingsStore::getHighlightTextColor() {
    return mHighlightTextColor;
}

vec3 ZSettingsStore::getBaseTextColor() {
    return mBaseTextColor;
}

void ZSettingsStore::setHighlightTextColor(vec3 color) {
    mHighlightTextColor = color;
}

void ZSettingsStore::setBaseTextColor(vec3 color) {
    mBaseTextColor = color;
}
