//
// Created by lukas on 1/24/22.
//

#ifndef ZPATH_DATASTORE_H
#define ZPATH_DATASTORE_H

#include <string>
#include "json/json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "utils/zutil.h"

#include <filesystem>
#if __APPLE__
using std::__fs::filesystem::directory_iterator;
using std::__fs::filesystem::directory_entry;
using std::__fs::filesystem::path;
using std::__fs::filesystem::relative;
#else
using std::filesystem::directory_iterator;
using std::filesystem::directory_entry;
using std::filesystem::path;
using std::filesystem::relative;

#endif


using namespace std;
using namespace nlohmann;

class DataStore {
public:
    static DataStore& getInstance(){
        static DataStore instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    static DataStore& get(){
        return getInstance();
    }

    struct Crumb {
        string mKey;
        int mIndex = -1;
    };

    json parseJsonFromFile(string path) {
        std::ifstream t(path);
        std::string dataString;
        t.seekg(0, std::ios::end);
        dataString.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        dataString.assign((std::istreambuf_iterator<char>(t)),
                          std::istreambuf_iterator<char>());

        json j = json::parse(dataString);

        return j;
    }

    json parseCsvFile(string path) {
        std::ifstream t(path);
        std::string dataString;
        t.seekg(0, std::ios::end);
        dataString.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        dataString.assign((std::istreambuf_iterator<char>(t)),
                          std::istreambuf_iterator<char>());
        json j = csvToJson(dataString);
        return j;
    }

    json csvToJson(string csv) {
        json allJson = json();

        vector<vector<string>> grid;
        vector<string> rows = ZUtil::split(csv, '\n');
        for (auto row : rows) {
            vector<string> tokens = ZUtil::split(row, ',');
            for (int i = 0; i < tokens.size(); i++) {
                tokens.at(i) = cleanString(tokens.at(i));
            }
            grid.push_back(tokens);
        }

        vector<int> cursor = vector<int>(grid.at(0).size(), 0);

        for (int i = 0; i < grid.size(); i++) {
            for (int j = 0; j < grid.at(i).size(); j++) {
                if (i >= cursor.at(j)) {

                    string token = grid.at(i).at(j);
                    string downOne = "";

                    if (grid.size() > i + 1) { downOne = grid.at(i + 1).at(j); }

                    if (ZUtil::isNumber(downOne) && !ZUtil::isNumber(token)) {
                        string key = token;
                        vector<float> values;

                        float number = stof(downOne);
                        int index = i;
                        while (index != -1) {
                            cursor.at(j) = index + 1;
                            values.push_back(number);

                            if (grid.size() > index + 1 && ZUtil::isNumber(grid.at(index + 1).at(j))) {
                                number = stof(grid.at(index + 1).at(j));
                                index++;
                            } else {
                                index = -1;
                            }
                        }

                        allJson[key] = values;
                    } else if (ZUtil::isNumber(downOne) && ZUtil::isNumber(token)) {
                        string key = to_string(i) + "," + to_string(j);
                        vector<float> values;
                        float number = stof(downOne);
                        int index = i + 1;
                        while (index != -1) {
                            cursor.at(j) = index;
                            values.push_back(number);

                            if (grid.size() > index + 1 && ZUtil::isNumber(grid.at(index + 1).at(j))) {
                                number = stof(grid.at(index + 1).at(j));
                                index++;
                            } else {
                                index = -1;
                            }

                        }
                        allJson[key] = values;
                    }
                }

            }
        }
        return allJson;
    }

    string cleanString(string potentialNumber) {
        if (potentialNumber.at(0) == '$') {
            return potentialNumber.substr(1, potentialNumber.size() - 1);
        }
        return potentialNumber;
    }

    /**
     * Parse json to float list and store
     * @param data Raw json data
     */
    void storeData(json data, const string& apath) {
        string binPath = ZSettings::get().getResourcePath();
        path binP = path(binPath);
        path p = path(apath);
        path rPath;
        if (!p.is_relative()) {
            rPath = relative(p, binPath);
        } else {
            rPath = p;
        }

        mDataList.push_back(rPath);
        mDataIndexMap.insert({mDataList.size() - 1, rPath});
        mJsonMap.insert({rPath, data});
        for (const auto& l : mDataChangeListeners) {
            l(rPath);
        }
    }

    void storeData(ZUtil::Image texture, string apath) {
        string binPath = ZSettings::get().getResourcePath();
        path binP = path(binPath);
        path p = path(apath);
        string rPath;
        if (!p.is_relative()) {
            rPath = relative(p, binPath);
        } else {
            rPath = p;
        }

        mDataList.push_back(rPath);
        mDataIndexMap.insert({mDataList.size() - 1, rPath});
        mTextureMap.insert({rPath, texture});

        for (const auto& l : mDataChangeListeners) {
            l(rPath);
        }
    }

    void setCrumbsForIndex(int index, const vector<Crumb>& crumbs) {
        mCrumbsMap.insert({index, crumbs});

        json data = mJsonMap.at(mDataIndexMap.at(index));
        vector<float> dataPoints = followCrumbs(data, crumbs, 0);
        if (mDataMap.count(mDataIndexMap.at(index)) > 0) {
            mDataMap.at(mDataIndexMap.at(index)) =  dataPoints;
        } else {
            mDataMap.insert({mDataIndexMap.at(index), dataPoints});
        }
    }

    bool crumbsValidForIndex(int index, const vector<Crumb>& crumbs) {
        json data = mJsonMap.at(mDataIndexMap.at(index));
        bool valid = crumbsValid(data, crumbs, 0);
        return valid;
    }

    vector<float> followCrumbs(json data, vector<Crumb> crumbs, int depth) {
        vector<float> dataList;

        if (depth >= crumbs.size()) {
            return vector<float>();
        }

        if (crumbs.at(depth).mIndex != -1) {
            // todo: check for array
            for (auto & datum : data) {
                if (datum.is_number_float()) {
                    dataList.push_back(datum);
                } else if (datum.is_object()) {
                    vector<float> someData = followCrumbs(datum, crumbs, depth + 1);
                    dataList.insert(dataList.end(), someData.begin(), someData.end());
                }
            }
        } else {
            vector<float> someData = followCrumbs(data[crumbs.at(depth).mKey], crumbs, depth + 1);
            dataList.insert(dataList.end(), someData.begin(), someData.end());
        }

        return dataList;
    }

    bool crumbsValid(json data, vector<Crumb> crumbs, int depth) {
        if (depth >= crumbs.size()) {
            return false;
        }

        bool isValid = true;

        if (crumbs.at(depth).mIndex != -1) {
            // todo: check for array

            if (data.empty() || !data.is_array()) {
                return false;
            }
            if (data.at(0).is_number_float()) {
                if (depth == crumbs.size() - 1) {
                    return true;
                }
            } else if (data.at(0).is_object()) {
                isValid &= crumbsValid(data.at(0), crumbs, depth + 1);
            }

        } else {
            isValid &= crumbsValid(data[crumbs.at(depth).mKey], crumbs, depth + 1);
        }

        return isValid;
    }

    float getDataAtIndex(unsigned int fileIndex, float x) {
        if (fileIndex != -1 && mDataList.size() > fileIndex) {
            string path = mDataList.at(fileIndex);
            if (mDataMap.count(path) > 0) {
                vector<float> points = mDataMap.at(path);
                float size = points.size() - 1;
                float leftX = std::min(size, std::max(0.0f, floor(x)));
                float rightX = std::min(size, std::max(0.0f, ceil(x)));
                float factor = x - floor(x);

                if (points.size() <= leftX || points.size() <= rightX) {
                    return 0;
                }
                float leftY = mDataMap.at(path).at((int) leftX);
                float rightY = mDataMap.at(path).at((int) rightX);

                return leftY + ((rightY - leftY) * factor);
            }
        }
        return 0;
    }

    vec4 getPixelAtIndex(unsigned int fileIndex, float x, float y) {
        if (fileIndex != -1 && mDataList.size() > fileIndex) {
            string path = mDataList.at(fileIndex);
            if (mTextureMap.count(path) > 0) {
                ZUtil::Image texture = mTextureMap.at(path);
                if (x >= 0 && x < texture.width && y >= 0 && y < texture.height) {
                    int index = (floor(x) + (floor(texture.height - y) * texture.width));
                    if (index < texture.width * texture.height) {

                        float r = texture.pixels[index * texture.compCount];
                        float g = texture.pixels[index * texture.compCount + 1];
                        float b = texture.pixels[index * texture.compCount + 2];
                        float a = 1;
                        if (texture.compCount >= 4) {
                            a = texture.pixels[index * texture.compCount + 3];
                        }
                        return vec4(r, g, b, a);
                    }
                }
            }
        }
        return vec4(0);
    }

    vector<string> getFileList() {
        return mDataList;
    }

    vector<string> getFileNameList() {
        vector<string> fileNameList;
        for (string path : mDataList) {
            vector<string> tokens = split(path, '/');
            fileNameList.push_back(tokens.at(tokens.size() - 1));
        }

        return fileNameList;
    }

    vector<string> split(const string &s, char delim) {
        char escape = '\\';
        vector<string> result;
        stringstream ss (s);
        string item;

        string last = "";
        while (getline(ss, item, delim)) {

            // Delimiter was escaped
            if (!last.empty() && !result.empty() &&
                last.at(last.size() - 1) == escape) {

                last = item;
                int rsize = result.size() - 1;
                result.at(rsize).erase( result.at(rsize).size() - 1);
                result.at(result.size() - 1) += delim + item;
            } else {
                result.push_back(item);
                last = item;
            }
        }
        return result;
    }

    void testJson() {
       // json j;
        //j["hello"] = "temp";
       /// cout << j.dump() << endl;
    }

    void addDataChangeListener(const function<void(string)>& l) {
        mDataChangeListeners.push_back(l);
    }

private:

    map<int, string> mDataIndexMap;
    map<string, vector<float>> mDataMap;
    map<string, json> mJsonMap;
    map<string, ZUtil::Image> mTextureMap;
    map<int, vector<Crumb>> mCrumbsMap;
    vector<string> mDataList;
    vector<function<void(string)>> mDataChangeListeners;
    DataStore() {}
};


#endif //ZPATH_DATASTORE_H
