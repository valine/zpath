
#include "utils/zimageutil.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>
#include "glad/glad.h"

#include <zlib.h>
#include "png.h"


void ZImageUtil::saveImage(const char *file, float *pixels, int w, int h) {
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
        png_bytep row = row_pointers[y];
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

void ZImageUtil::saveGlTex(const char *file, unsigned int tex, int w, int h) {
    int size = w * h;
    float bytes[4 * size];
    glBindTexture(GL_TEXTURE_2D, tex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, bytes);
    saveImage(file, bytes, w, h);
}

void ZImageUtil::saveGlFBO(const char *file, unsigned int fbo, int w, int h) {
    int size = w * h;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    float bytes[4 * size];
    glReadPixels(0,0,w,h,GL_RGBA, GL_FLOAT, bytes);
    saveImage(file, bytes, w, h);
}

