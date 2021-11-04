#ifndef ZTEXTURE_H
#define ZTEXTURE_H

#include "glad/glad.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

class ZTexture {

public:

    ZTexture(int id, int idDark);

    enum FillMode {
        stretch,
        clip
    };


	ZTexture(string path);
	ZTexture(int id);
	ZTexture(float* pixels, int width, int height);
	unsigned int loadTexture(char const * path);
	unsigned int loadTexture(float* pixels, int width, int height);
	uint getID();
	void updateTexture(float* pixels, int width, int height);
	void setID(int);
    void setID(int light, int dark);
	void save();

	void setScaleOffset(float scale, vec2 offset) {
	    mOffset = offset;
	    mScale = scale;
	}

    int mWidth = 0;
    int mHeight = 0;

    vec2 mOffset = vec2(0);
    float mScale = 1;

     FillMode mFillMode = stretch;

     void setFillMode(FillMode fillMode) {
         mFillMode = fillMode;
     }

    uint getIDDark();

private:
	uint mTextureID;
    uint mTextureIDDark;

};

#endif

