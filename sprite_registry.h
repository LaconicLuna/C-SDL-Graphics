#ifndef SPRITE_REGISTRY_H
#define SPRITE_REGISTRY_H

#include "texture.h"
void LocalLoadPNG(const char* filename, Texture* outTex);
void RegisterSprite(const char* name, const char* filename);
Texture* GetSprite(const char* name);
void FreeSprites(void);

#endif
