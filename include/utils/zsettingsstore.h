//
// Created by lukas on 7/14/19.
//

#ifndef ZPATH_ZSETTINGSSTORE_H
#define ZPATH_ZSETTINGSSTORE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

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
        static ZSettingsStore instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    vec4 getHighlightColor();
    vec4 getBaseColor();
    vec4 getInactiveColor();
    vec4 getBackgroundColor();
    vec3 getHighlightTextColor();
    vec3 getBaseTextColor();
    string getResourcePath();
    void setResourcePath(string path);

    void setHighlightColor(vec4);
    void setBaseColor(vec4);
    void setInactiveColor(vec4);
    void setBackgroundColor(vec4);
    void setHighlightTextColor(vec3);
    void setBaseTextColor(vec3);
private:
    vec4 mBaseColor = vec4(0.9, 0.9, 0.92, 1.0);
    vec4 mBackgroundColor = vec4(0.85, 0.85, 0.86, 1.0);
    vec4 mHighlightColor = vec4(0.005858, 0.253008, 0.698193, 1);
    vec4 mInactiveColor = vec4(0.0,0.0,0.0,0.5);

    vec3 mHighlightTextColor = vec3(1.0,1.0,1.0);
    vec3 mBaseTextColor = vec3(0,0,0);

    string mResourcePath;
    ZSettingsStore();
    static ZSettingsStore *mInstance;
    ZSettingsStore(ZSettingsStore const&);              // Don't Implement
    void operator=(ZSettingsStore const&); // Don't implement
};


#endif //ZPATH_ZSETTINGSSTORE_H
