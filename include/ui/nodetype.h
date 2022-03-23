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
#include "ui/zview.h"
#include "ui/charttype.h"
#include "ui/sockettype.h"

using namespace glm;
using namespace std;
using namespace nlohmann;

class ZNodeView;

class NodeType {

public:
    string mName;
    vec2 mSocketCount;
    vector<vector<SocketType>> mSocketType;
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
    ChartType mChartType;
    bool mShowInDrawer;

    NodeType() {}

    std::function<vector<vector<float>>(
            vector<vector<float>> x,
            vector<vector<float>> rootInput,
            const vector<complex<float>>& cache,
            float chartStart,
            float chartWidth,
            ZNodeView* node)> mCompute = nullptr;

    vector<function<void(ZView* sender)>> mOnButtonClick;

    void setCompute(std::function<vector<vector<float>>(
            vector<vector<float>> x, vector<vector<float>> rootInput,
            const vector<complex<float>>& cache,
            float chartStart,
            float chartWidth,
            ZNodeView* node)> compute) {
        mCompute = std::move(compute);
    }

    function<void(ZView* sender)> getButtonCallback(int index) {
        return mOnButtonClick.at(index);
    }

    static NodeType* fromFile(string path, std::function<vector<vector<float>>(
            vector<vector<float>> x, vector<vector<float>> rootInput,
            const vector<complex<float>>& cache,
            float chartStart,
            float chartWidth,
            ZNodeView* node)> compute) {
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
        NodeType* type = fromJSON(j);;
        type->setCompute(std::move(compute));
        return type;
    }

    static NodeType* fromFile(string path) {
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

    static NodeType* fromJSON(json j) {
        auto* nodeType = new NodeType();
        nodeType->mName = j["name"];

        vector<SocketType> inputs;
        for (string type : j["socketType"]["input"]) {
            inputs.push_back(socketTypeFromString(type));
        }

        vector<SocketType> outputs;
        for (string type : j["socketType"]["output"]) {
            outputs.push_back(socketTypeFromString(type));
        }

        nodeType->mSocketCount.x = inputs.size();
        nodeType->mSocketCount.y = outputs.size();
        nodeType->mIsDynamicSocket = j["isDynamicSocket"];
        nodeType->mIsGroupNode = j["isGroupNode"];
        for (int i = 0; i < (int) nodeType->mSocketCount.x; i++) {
            nodeType->mDefaultInput.push_back(j["defaultInput"][i]);
            nodeType->mSocketNames.push_back(j["socketNames"][i]);
        }

        for (const json& inner : j["defaultMagnitude"]) {
            nodeType->mDefaultMagnitude.push_back(inner);
        }

        for (const json& enums : j["enumNames"]) {
            vector<string> names;
            for (const json& aEnum : enums) {
                names.push_back(aEnum);
            }
            nodeType->mEnumNames.push_back(names);
        }

        nodeType->mNodeSize.x = j["nodeSize"][0];
        nodeType->mNodeSize.y = j["nodeSize"][1];

        nodeType->mChartBounds.x = j["chartBounds"][0];
        nodeType->mChartBounds.y = j["chartBounds"][1];
        nodeType->mChartBounds.z = j["chartBounds"][2];
        nodeType->mChartBounds.a = j["chartBounds"][3];

        nodeType->mIsOutputLabelVisible = j["isOutputLabelVisible"];
        nodeType->mIsDropDownVisible = j["isDropDownVisible"];

        for (const json& button : j["buttonNames"]) {
            nodeType->mButtonNames.push_back(button);
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
        nodeType->mColor = color;

        nodeType->mAdaptiveRes = j["adaptiveResolution"];

        string charttpye = j["chartType"];
        if (charttpye == "LINE_1D") {
            nodeType->mChartType = LINE_1D;
        } else if (charttpye == "LINE_1D_2X") {
            nodeType->mChartType = LINE_1D_2X;
        } else if (charttpye == "LINE_2D") {
            nodeType->mChartType = LINE_2D;
        } else if (charttpye == "IMAGE") {
            nodeType->mChartType = IMAGE;
        } else if (charttpye == "TEXT_FIELD") {
            nodeType->mChartType = TEXT_FIELD;
        }

        nodeType->mShowInDrawer = j["showInDrawer"];


        vector<vector<SocketType>> socketTypes = {inputs, outputs};
        nodeType->mSocketType = socketTypes;

        return nodeType;
    }


    static SocketType socketTypeFromString(string jString) {
        if (jString == "NONE") {
            return NONE;
        } else if (jString == "CON") {
            return CON;
        } else if (jString == "VAR") {
            return VAR;
        } else if (jString == "VAR_Z") {
            return VAR_Z;
        } else if (jString == "ENUM") {
            return ENUM;
        } else if (jString == "NN") {
            return NN;
        } else if (jString == "SYMBOLIC") {
            return SYMBOLIC;
        } else if (jString == "GROUP_SOCKET") {
            return GROUP_SOCKET;
        } else if (jString == "TEXT") {
            return TEXT;
        }
        return SocketType::NONE;
    }


};


#endif //ZPATH_NODETYPE_H
