
#include "utils/zutil.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>
#include "glad/glad.h"

#include <zlib.h>
#include <ui/zchartrenderer.h>
#include <pwd.h>
#include "png.h"

#define STB_IMAGE_IMPLEMENTATION
#include "ui/stb_image.h"

string ZUtil::replace(std::string subject, const std::string& search,
                            const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}


void ZUtil::saveImage(const char *file, float *pixels, int w, int h) {
    /* create file */
    FILE *fp = fopen(file, "wb");
    if (!fp) {
        cout << "error opening file" << endl;
    }
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, fp);

    png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * h);
    for(int y = 0; y < h; y++) {
        row_pointers[y] = (png_byte*)malloc((sizeof(png_bytep) * w));
    }

    int i = 0;
    for(int y = h - 1; y >= 0; y--) {
        png_bytep row = row_pointers[h - y - 1];
        for(int x = 0; x < w; x++) {
            row[x * 4] = (int) (pixels[i] * 255);
            row[x * 4 + 1] = (int) (pixels[i + 1] * 255);
            row[x * 4 + 2] = (int) (pixels[i + 2] * 255);
            row[x * 4 + 3] = (int) (pixels[i + 3] * 255);

            i+=4;
        }
    }

    png_set_IHDR(png_ptr, info_ptr, w, h,
                 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, NULL);

    for (int y=0; y<h; y++)
        free(row_pointers[y]);
    free(row_pointers);

    fclose(fp);

}

void ZUtil::saveImageR(const char *file, float *pixels, int w, int h) {
    float max = 0;
    for (uint i = 0; i < w * h; i++) {
        if (pixels[i] > max) {
            max = pixels[i];
        }
    }

    for (uint i = 0; i < w * h; i++) {
        pixels[i] = pixels[i] / max;
    }

    /* create file */
    FILE *fp = fopen(file, "wb");
    if (!fp) {
        cout << "error opening file" << endl;
    }
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, fp);

    png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * h);
    for(int y = 0; y < h; y++) {
        row_pointers[y] = (png_byte*)malloc((sizeof(png_bytep) * w));
    }

    int i = 0;
    for(int y = 0; y < h; y++) {
        png_bytep row = row_pointers[h - y - 1];
        for(int x = 0; x < w; x++) {
            row[x * 4] = (int) (pixels[i] * 255);
            row[x * 4 + 1] = (int) (0);
            row[x * 4 + 2] = (int) (0);
            row[x * 4 + 3] = (int) (255);

            i+=1;
        }
    }

    png_set_IHDR(png_ptr, info_ptr, w, h,
                 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, NULL);

    for (int y=0; y<h; y++)
        free(row_pointers[y]);
    free(row_pointers);

    fclose(fp);
}


void ZUtil::saveImage(const char *file, double *pixels, int w, int h) {
    float floats[w * h];
    for (uint i = 0; i < w * h; i++) {
        floats[i] = (float) pixels[i];
    }

    saveImage(file, floats, w, h);
}

void ZUtil::saveImageR(const char *file, double *pixels, int w, int h) {
    float floats[w * h];
    for (uint i = 0; i < w * h; i++) {
        floats[i] = (float) pixels[i];
    }

    saveImageR(file, floats, w, h);
}

void ZUtil::saveGlTex(const char *file, unsigned int tex, int w, int h) {
    int size = w * h * 4;
    auto* bytes = (float*) malloc(size * sizeof(float));
    glBindTexture(GL_TEXTURE_2D, tex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, bytes);
    saveImage(file, bytes, w, h);
    free(bytes);
}

void ZUtil::saveGlFBO(const char *file, unsigned int fbo, int w, int h) {
    int size = w * h * 4;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    auto* bytes = (float*) malloc(size * sizeof(float));
    glReadPixels(0,0,w,h,GL_RGBA, GL_FLOAT, bytes);
    saveImage(file, bytes, w, h);
    free(bytes);
}

void ZUtil::saveGlFBOMain(const char *file, int x, int y, int w, int h) {
    int size = w * h * 4;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D,0);
    auto* bytes = (float*) malloc(size * sizeof(float));
    glReadPixels(x,y,w,h,GL_RGBA, GL_FLOAT, bytes);
    saveImage(file, bytes, w, h);
    free(bytes);
}

void ZUtil::saveView(ZView *v) {
    saveGlFBOMain((getHomeFolder() + "/Desktop/view.png").c_str(), v->getLeft(),
                  v->getWindowHeight() - v->getBottom(), v->getWidth(), v->getHeight());
}

void ZUtil::chart(float *points, int size, float max) {
    chart((getHomeFolder() + "/Desktop/chart").c_str(), points, size, max);
}

void ZUtil::chart(const char *file, float *p, int s, float max) {
    int w = 400;
    int h = 600;

    string filename = file;

    ZChartRenderer renderer = ZChartRenderer(w, h);
    renderer.addLine(p, s);

    if (max != -1) {
        renderer.setMax(max);
    }
    renderer.onDraw();

    filename+="MIN=" + to_string(renderer.getMin()) + " MAX=" + to_string(renderer.getMax()) + ".png";
    saveGlTex(filename.c_str(), renderer.getTexID(), w, h);
}

void ZUtil::chart(const char *file, double *p, int s, float max) {
    float pointsF[s];
    for (int i =0; i < s; i++) {
        pointsF[i] = p[i];
    }
    chart(file, pointsF, s, max);
}

void ZUtil::chart(double *points, int size, float max) {
    time_t thetime;
    time(&thetime);

    std::string s = getHomeFolder() + "/Desktop/chart/chart" + std::to_string(thetime) + "---";
    char const *pchar = s.c_str();
    chart(pchar, points, size, max);
}

void ZUtil::chart(double *p, int size, int mod, int offset, float max) {
    vector<double> values;
    for (int i = 0; i < size; i++) {
        if (i % mod == offset) {
            values.push_back(p[i]);
        }
    }
    chart(values.data(), size / mod, max);
}

string ZUtil::getHomeFolder() {
    const char *homedir;

    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    return homedir;
}

ZUtil::Image ZUtil::loadTexture(const char * path) {
    int width, height, nrComponents;
    float *data = stbi_loadf(path, &width, &height, &nrComponents, 0);
    Image img;
    img.compCount = nrComponents;
    img.pixels = data;
    img.width = width;
    img.height = height;
    return img;
}

void ZUtil::freeStbi(float* data) {
    stbi_image_free(data);
}


