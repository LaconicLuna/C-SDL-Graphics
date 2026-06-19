#ifndef RENDERER_H
#define RENDERER_H
#include "texture.h"
#include <stdint.h>
extern uint32_t framebuffer[];

void PutPixel(int x, int y, uint32_t color);
void PutPixelAlpha(int x, int y, uint32_t color, float alpha);

void DrawSprite(int x, int y, const Texture* tex);
void DrawSpriteScaled(int x, int y, const Texture* tex, float scale);

void DrawChar(int x, int y, char c, uint8_t r, uint8_t g, uint8_t b);
void DrawString(int x, int y, const char* str, uint8_t r, uint8_t g, uint8_t b);

void renderer_init(void);
void renderer_render();
int  renderer_should_close(void);
void renderer_close(void);


#endif