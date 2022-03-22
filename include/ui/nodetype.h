//
// Created by lukas on 3/21/22.
//

#ifndef ZPATH_NODETYPE_H
#define ZPATH_NODETYPE_H
#include <string>
#include <utility>
#include <vector>
#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ui/zcolor.h"
#include "json/json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <mutex>
#include <complex>
#include "utils/zsettingsstore.h"

using namespace glm;
using namespace std;
using namespace nlohmann;

class NodeType {

public:
    string mName;
    vec2 mSocketCount;
    bool mIsDynamicSocket;
    bool mIsGroupNode;
    vector<float> mDefaultInput;
    vector<int> mDefaultMagnitude;
    vector<string> mSocketNames;
    vector<vector<string>> mEnumNames;
    vec2 mNodeSize;
    vec4 mChartBounds;
    bool mIsOutputLabelVisible;
    bool mIsDropDownVisible;
    vector<string> mButtonNames;
    ZColor mColor = ZColor(vec4(1));
    bool mAdaptiveRes;
    string mChartType;
    bool mShowInDrawer;

    NodeType() {}

    std::function<vector<vector<float>>(
            vector<vector<float>> x,
            vector<vector<float>> rootInput,
            vector<float>& cache,
            float chartStart,
            float chartWidth)> mCompute;

    void setCompute(std::function<vector<vector<float>>(
            vector<vector<float>> x, vector<vector<float>> rootInput,
            vector<float>& cache,
            float chartStart,
            float chartWidth)> compute) {
        mCompute = std::move(compute);
    }


    static NodeType fromFile(string path, std::function<vector<vector<float>>(
            vector<vector<float>> x, vector<vector<float>> rootInput,
            vector<float>& cache,
            float chartStart,
            float chartWidth)> compute) {
        string resources = ZSettings::get().getResourcePath();
        string fullPath = resources + "resources/node-def/" + path;
        std::ifstream t(fullPath);
        std::string dataString;
        t.seekg(0, std::ios::end);
        dataString.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        dataString.assign((std::istreambuf_iterator<char>(t)),
                          std::istreambuf_iterator<char>());
        json j = json::parse(dataString);
        NodeType type = fromJSON(j);;
        type.setCompute(std::move(compute));
        return type;
    }

    static NodeType fromFile(string path) {
        string resources = ZSettings::get().getResourcePath();
        string fullPath = resources + "resources/node-def/" + path;
        std::ifstream t(fullPath);
        std::string dataString;
        t.seekg(0, std::ios::end);
        dataString.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        dataString.assign((std::istreambuf_iterator<char>(t)),
                          std::istreambuf_iterator<char>());
        json j = json::parse(dataString);
        return fromJSON(j);
    }

    static NodeType fromJSON(json j) {
        NodeType nodeType = NodeType();
        nodeType.mName = j["name"];
        nodeType.mSocketCount.x = j["socketCount"][0];
        nodeType.mSocketCount.y = j["socketCount"][1];
        nodeType.mIsDynamicSocket = j["isDynamicSocket"];
        nodeType.mIsGroupNode = j["isGroupNode"];
        for (int i = 0; i < (int) nodeType.mSocketCount.x; i++) {
            nodeType.mDefaultInput.push_back(j["defaultInput"][i]);
            nodeType.mSocketNames.push_back(j["socketNames"][i]);
        }

        for (const json& inner : j["defaultMagnitude"]) {
            nodeType.mDefaultMagnitude.push_back(inner);
        }

        for (const json& enums : j["enumNames"]) {
            vector<string> names;
            for (const json& aEnum : enums) {
                names.push_back(aEnum);
            }
            nodeType.mEnumNames.push_back(names);
        }

        nodeType.mNodeSize.x = j["nodeSize"][0];
        nodeType.mNodeSize.y = j["nodeSize"][1];

        nodeType.mChartBounds.x = j["chartBounds"][0];
        nodeType.mChartBounds.y = j["chartBounds"][1];
        nodeType.mChartBounds.z = j["chartBounds"][2];
        nodeType.mChartBounds.a = j["chartBounds"][3];

        nodeType.mIsOutputLabelVisible = j["isOutputLabelVisible"];
        nodeType.mIsDropDownVisible = j["isDropDownVisible"];

        for (const json& button : j["buttonNames"]) {
            nodeType.mButtonNames.push_back(button);
        }

        vec4 colorLight;
        colorLight.r = j["colorLight"][0];
        colorLight.g = j["colorLight"][1];
        colorLight.b = j["colorLight"][2];
        colorLight.a = j["colorLight"][3];

        vec4 colorDark;
        colorDark.r = j["colorDark"][0];
        colorDark.g = j["colorDark"][1];
        colorDark.b = j["colorDark"][2];
        colorDark.a = j["colorDark"][3];

        ZColor color = ZColor(colorLight, colorDark);
        nodeType.mColor = color;

        nodeType.mAdaptiveRes = j["adaptiveResolution"];
        nodeType.mChartType = j["chartType"];
        nodeType.mShowInDrawer = j["showInDrawer"];

        return nodeType;
    }

};


#endif //ZPATH_NODETYPE_H
