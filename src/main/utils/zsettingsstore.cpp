//
// Created by lukas on 7/14/19.
//

#include <iostream>
#include <fstream>
#include "utils/zsettingsstore.h"
#include "utils/enums/colormode.h"
using namespace std;

ZSettings::ZSettings() {

}

ZColor ZSettings::getHighlightColor() {
    return mHighlightColor;
}
ZColor ZSettings::getBaseColor() {
    return mBaseColor;
}

void ZSettings::setBaseColor(ZColor color) {
    mBaseColor = color;
}

void ZSettings::setHighlightColor(ZColor color) {
    mHighlightColor = color;
}

ZColor ZSettings::getInactiveColor() {
    return mInactiveColor;
}

void ZSettings::setInactiveColor(ZColor color) {
    mInactiveColor = color;
}

ZColor ZSettings::getBackgroundColor() {
    return mBackgroundColor;
}

void ZSettings::setBackgroundColor(ZColor color) {
    mBackgroundColor = color;
}

ZColor ZSettings::getHighlightTextColor() {
    return mHighlightTextColor;
}

ZColor ZSettings::getBaseTextColor() {
    return mBaseTextColor;
}

void ZSettings::setHighlightTextColor(ZColor color) {
    mHighlightTextColor = color;
}

void ZSettings::setBaseTextColor(ZColor color) {
    mBaseTextColor = color;
}

string ZSettings::getResourcePath() {
    return mResourcePath;
}

void ZSettings::setResourcePath(string path) {
    mResourcePath = path;
}

void ZSettings::setColorMode(ColorMode mode) {

    string projectFolder = "resources/settings/";
    string path = ZSettings::get().getResourcePath() + projectFolder;
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

ColorMode ZSettings::getColorMode() {
    string projectFolder = "resources/settings/";
    string path = ZSettings::get().getResourcePath() + projectFolder;
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

void ZSettings::setWheelMode(WheelMode mode) {
    // Todo switch to JSON when we need more options in settings
    string projectFolder = "resources/settings/";
    string path = ZSettings::get().getResourcePath() + projectFolder;
    string name = "settings";
    string ext = ".csv";
    string fullPathString = path + name + ext;

    ofstream out(fullPathString);
    switch (mode) {
        case zoom:
            out << ZOOM;
            break;
        case scroll:
            out << SCROLL;
            break;
    }
    out.close();
}

WheelMode ZSettings::getWheelMode() {
    string projectFolder = "resources/settings/";
    string path = ZSettings::get().getResourcePath() + projectFolder;
    string name = "settings";
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
        case ZOOM:
            return zoom;
        case SCROLL:
            return scroll;
    }
}

void ZSettings::setComputationDevice(CompDevice cd) {
    mCompDevice = cd;

    // Todo switch to JSON when we need more options in settings
    string projectFolder = "resources/settings/";
    string path = ZSettings::get().getResourcePath() + projectFolder;
    string name = "compdevice";
    string ext = ".csv";
    string fullPathString = path + name + ext;

    ofstream out(fullPathString);
    switch (cd) {
        case glsl:
            out << GLSL;
            break;
        case cpu:
            out << CPU;
            break;
    }
    out.close();
}

CompDevice ZSettings::getCompDevice() {
    if (mCompDevice == unset) {
        string projectFolder = "resources/settings/";
        string path = ZSettings::get().getResourcePath() + projectFolder;
        string name = "compdevice";
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
            case CPU:
                mCompDevice = cpu;
                return cpu;
            case GLSL:
                mCompDevice = glsl;
                return glsl;
        }
    }

    return mCompDevice;
}
