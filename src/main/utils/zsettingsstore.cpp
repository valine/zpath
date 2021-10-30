//
// Created by lukas on 7/14/19.
//

#include <iostream>
#include <fstream>
#include "utils/zsettingsstore.h"
#include "utils/colormode.h"
using namespace std;

ZSettingsStore::ZSettingsStore() {

}

ZColor ZSettingsStore::getHighlightColor() {
    return mHighlightColor;
}
ZColor ZSettingsStore::getBaseColor() {
    return mBaseColor;
}

void ZSettingsStore::setBaseColor(ZColor color) {
    mBaseColor = color;
}

void ZSettingsStore::setHighlightColor(ZColor color) {
    mHighlightColor = color;
}

ZColor ZSettingsStore::getInactiveColor() {
    return mInactiveColor;
}

void ZSettingsStore::setInactiveColor(ZColor color) {
    mInactiveColor = color;
}

ZColor ZSettingsStore::getBackgroundColor() {
    return mBackgroundColor;
}

void ZSettingsStore::setBackgroundColor(ZColor color) {
    mBackgroundColor = color;
}

ZColor ZSettingsStore::getHighlightTextColor() {
    return mHighlightTextColor;
}

ZColor ZSettingsStore::getBaseTextColor() {
    return mBaseTextColor;
}

void ZSettingsStore::setHighlightTextColor(ZColor color) {
    mHighlightTextColor = color;
}

void ZSettingsStore::setBaseTextColor(ZColor color) {
    mBaseTextColor = color;
}

string ZSettingsStore::getResourcePath() {
    return mResourcePath;
}

void ZSettingsStore::setResourcePath(string path) {
    mResourcePath = path;
}

void ZSettingsStore::setColorMode(ColorMode mode) {

    string projectFolder = "resources/settings/";
    string path = ZSettingsStore::get().getResourcePath() + projectFolder;
    string name = "theme";
    string ext = ".csv";
    string fullPathString = path + name + ext;

    ofstream out(fullPathString);
    switch (mode) {
        case light:
            out << LIGHT;
            break;
        case dark:
            out << DARK;
            break;
    }
    out.close();

    if (mOnThemeChange != nullptr) {
        mOnThemeChange();
    }
}

ColorMode ZSettingsStore::getColorMode() {
    string projectFolder = "resources/settings/";
    string path = ZSettingsStore::get().getResourcePath() + projectFolder;
    string name = "theme";
    string ext = ".csv";
    string fullPathString = path + name + ext;

    std::ifstream t(fullPathString);
    std::string dataString;
    t.seekg(0, std::ios::end);
    dataString.reserve(t.tellg());
    t.seekg(0, std::ios::beg);
    dataString.assign((std::istreambuf_iterator<char>(t)),
                      std::istreambuf_iterator<char>());

    int index = stoi(dataString);
    switch (index) {
        case LIGHT:
            return light;
        case DARK:
            return dark;
    }
}
