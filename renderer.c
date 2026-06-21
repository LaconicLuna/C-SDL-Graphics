#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "font.h"
#include "texture.h"
#include "sprite_registry.h"


#define SCREEN_WIDTH  384
#define SCREEN_HEIGHT 240
#define SCALE         3
#define FB_WIDTH  (SCREEN_WIDTH * SCALE)
#define FB_HEIGHT (SCREEN_HEIGHT * SCALE)


static uint32_t framebuffer[FB_WIDTH * FB_HEIGHT];

int CRTeffects = 1;

// SDL3 Globals
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;


void PutPixel(int x, int y, uint32_t color) {
    for (int sy = 0; sy < SCALE; sy++) {
        for (int sx = 0; sx < SCALE; sx++) {
            int px = x * SCALE + sx;
            int py = y * SCALE + sy;
            framebuffer[py * FB_WIDTH + px] = color;
        }
    }
}

void PutPixelAlpha(int x, int y, uint32_t color, float alpha) {
    for (int sy = 0; sy < SCALE; sy++) {
        for (int sx = 0; sx < SCALE; sx++) {
        int px = x * SCALE + sx;
        int py = y * SCALE + sy;
        uint32_t bg = framebuffer[py * FB_WIDTH + px];

        uint8_t rB = (bg >> 16) & 0xFF;
        uint8_t gB = (bg >> 8) & 0xFF;
        uint8_t bB = bg & 0xFF;

        uint8_t rF = (color >> 16) & 0xFF;
        uint8_t gF = (color >> 8) & 0xFF;
        uint8_t bF = color & 0xFF;

        uint8_t r = rF * alpha + rB * (1 - alpha);
        uint8_t g = gF * alpha + gB * (1 - alpha);
        uint8_t b = bF * alpha + bB * (1 - alpha);

        framebuffer[py * FB_WIDTH + px] = (r << 16) | (g << 8) | b;
        }
    }
}

void DrawSprite(int x, int y, const Texture* tex) {
    for (int py = 0; py < tex->height; py++) {
        int sy = y + py;
        if (sy < 0 || sy >= SCREEN_HEIGHT) continue;

        for (int px = 0; px < tex->width; px++) {
            int sx = x + px;
            if (sx < 0 || sx >= SCREEN_WIDTH) continue;

            uint32_t color = tex->pixels[py * tex->width + px];
            uint8_t a = (color >> 24) & 0xFF;
            if (a == 0) continue;

            PutPixelAlpha(sx, sy, color, a / 255.0f);
        }
    }
}



void DrawChar(int x, int y, char c, uint8_t r, uint8_t g, uint8_t b) {
    if (c >= 'a' && c <= 'z') c -= 32;

    const unsigned char* glyph = font8x8[(unsigned char)c];

    for (int ky = 0; ky < 8; ky++) {
        int sy = y + ky;
        if (sy < 0 || sy >= SCREEN_HEIGHT) continue;

        for (int kx = 0; kx < 8; kx++) {
            int sx = x + kx;
            if (sx < 0 || sx >= SCREEN_WIDTH) continue;

            if (glyph[ky] & (1 << kx))
                PutPixel(sx, sy, (r << 16) | (g << 8) | b);
        }
    }
}


void DrawString(int x, int y, const char* str, uint8_t r, uint8_t g, uint8_t b) {
    while (*str) {
        DrawChar(x, y, *str, r, g, b);
        x += 8;
        str++;
    }
}

void renderer_init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return;
    }
    if (!SDL_CreateWindowAndRenderer("Virtual Machine", FB_WIDTH, FB_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("Window/Renderer creation failed: %s", SDL_GetError());
        return;
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, FB_WIDTH, FB_HEIGHT);
    if (!texture) {
        SDL_Log("Texture creation failed: %s", SDL_GetError());
        return;
    }
    SDL_StartTextInput(window);
}
#include <stddef.h>
#include <stdint.h>

void ClearFramebuffer(uint32_t color) {
    for (size_t i = 0; i < FB_WIDTH * FB_HEIGHT; i++)
        framebuffer[i] = color;
}

void renderer_render() {
    memset(framebuffer, 0, sizeof(framebuffer));
    ClearFramebuffer(0xFF0FADED);
    DrawSprite(0,0,GetSprite("smol"));
    if (CRTeffects == 1) {
        for (int y = 0; y < FB_HEIGHT; y++) {
            int band = y % 3;  
            for (int x = 0; x < FB_WIDTH; x++) {
                uint32_t c = framebuffer[y * FB_WIDTH + x];
                uint8_t r = (c >> 16) & 0xFF;
                uint8_t g = (c >> 8)  & 0xFF;
                uint8_t b =  c        & 0xFF;
                switch (band) {
                    case 0: 
                        g = g * 0.5f; b = b * 0.5f;
                        break;
                    case 1: 
                        r = r * 0.5f; b = b * 0.5f;
                        break;
                    case 2: 
                        r = r * 0.5f; g = g * 0.5f;
                        break;
                }
                framebuffer[y * FB_WIDTH + x] = (r << 16) | (g << 8) | b;
            }
        }
        
        // Horizontal NTSC-style color bleed
        static uint32_t temp[FB_WIDTH * FB_HEIGHT];
        memcpy(temp, framebuffer, sizeof(temp));
        for (int y = 0; y < FB_HEIGHT; y++) {
            for (int x = 1; x < FB_WIDTH - 1; x++) {
                uint32_t c0 = temp[y * FB_WIDTH + (x - 1)];
                uint32_t c1 = temp[y * FB_WIDTH + x];
                uint32_t c2 = temp[y * FB_WIDTH + (x + 1)];
                int r = ((c0 >> 16) & 0xFF) * 0.25 + ((c1 >> 16) & 0xFF) * 0.50 + ((c2 >> 16) & 0xFF) * 0.25;
                int g = ((c0 >> 8)  & 0xFF) * 0.25 + ((c1 >> 8)  & 0xFF) * 0.50 + ((c2 >> 8)  & 0xFF) * 0.25;
                int b = ( c0        & 0xFF) * 0.25 + ( c1        & 0xFF) * 0.50 + ( c2        & 0xFF) * 0.25;
                framebuffer[y * FB_WIDTH + x] = (r << 16) | (g << 8) | b;
            }
        }
        
        // Brightness-dependent horizontal bloom
        memcpy(temp, framebuffer, sizeof(temp));
        for (int y = 0; y < FB_HEIGHT; y++) {
            for (int x = 1; x < FB_WIDTH - 1; x++) {
                uint32_t c0 = temp[y * FB_WIDTH + (x - 1)];
                uint32_t c1 = temp[y * FB_WIDTH + x];
                uint32_t c2 = temp[y * FB_WIDTH + (x + 1)];
                int r0 = (c0 >> 16) & 0xFF; int g0 = (c0 >> 8) & 0xFF; int b0 = c0 & 0xFF;
                int r1 = (c1 >> 16) & 0xFF; int g1 = (c1 >> 8) & 0xFF; int b1 = c1 & 0xFF;
                int r2 = (c2 >> 16) & 0xFF; int g2 = (c2 >> 8) & 0xFF; int b2 = c2 & 0xFF;
                float lum = (r1 + g1 + b1) / 3.0f / 255.0f;
                float bloom = lum * 0.35f;  
                int r = r1 + (int)((r0 + r2 - 2*r1) * bloom);
                int g = g1 + (int)((g0 + g2 - 2*g1) * bloom);
                int b = b1 + (int)((b0 + b2 - 2*b1) * bloom);
                if (r > 255) r = 255; if (r < 0) r = 0;
                if (g > 255) g = 255; if (g < 0) g = 0;
                if (b > 255) b = 255; if (b < 0) b = 0;
                framebuffer[y * FB_WIDTH + x] = (r << 16) | (g << 8) | b;
            }
        }
    }
    SDL_UpdateTexture(texture, NULL, framebuffer, FB_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL); 
    SDL_RenderPresent(renderer);
}

int renderer_should_close() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return 1;
        }
    }
    return 0;
}

void renderer_close() {
    SDL_StopTextInput(window);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
