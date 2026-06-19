#ifndef TEXTURE_H
#define TEXTURE_H
#include <stdint.h>
typedef struct {
    int width;
    int height;
    uint32_t* pixels;
} Texture;

#endif
