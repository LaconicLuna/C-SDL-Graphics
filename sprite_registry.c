#include "texture.h"
#include "stb_image.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_SPRITES 1024

typedef struct {
    char name[32];
    Texture tex;
} SpriteEntry;

static SpriteEntry gSprites[MAX_SPRITES];
static int gSpriteCount = 0;


void LocalLoadPNG(const char* filename, Texture* outTex) {
    int channels;
    uint8_t* raw = stbi_load(filename, &outTex->width, &outTex->height, &channels, 4);
    if (!raw) {
        fprintf(stderr, "Failed to load PNG: %s\n", filename);
        outTex->width = 16;
        outTex->height = 16;
        outTex->pixels = malloc(sizeof(uint32_t) * 16 * 16);
        for (int i = 0; i < 16 * 16; i++)
            outTex->pixels[i] = 0xFFFF00FF;  // magenta fallback
        return;
    }

    // Allocate final pixel buffer
    outTex->pixels = malloc(sizeof(uint32_t) * outTex->width * outTex->height);
    if (!outTex->pixels) {
        stbi_image_free(raw);
        return;
    }

    // Convert RGBA → ARGB8888
    for (int i = 0; i < outTex->width * outTex->height; i++) {
        uint8_t r = raw[i * 4 + 0];
        uint8_t g = raw[i * 4 + 1];
        uint8_t b = raw[i * 4 + 2];
        uint8_t a = raw[i * 4 + 3];
        outTex->pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
    }

    stbi_image_free(raw);
}

void RegisterSprite(const char* name, const char* filename) {
    if (gSpriteCount >= MAX_SPRITES) return;
    strcpy(gSprites[gSpriteCount].name, name);
    LocalLoadPNG(filename, &gSprites[gSpriteCount].tex);
    gSpriteCount++;
}

Texture* GetSprite(const char* name) {
    for (int i = 0; i < gSpriteCount; i++) {
        if (strcmp(gSprites[i].name, name) == 0)
            return &gSprites[i].tex;
    }
    return NULL;
}

void FreeSprites() {
    for (int i = 0; i < gSpriteCount; i++) {
        stbi_image_free(gSprites[i].tex.pixels);
    }
    gSpriteCount = 0;
}