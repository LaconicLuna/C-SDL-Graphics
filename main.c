#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "renderer.h"
#include "sprite_registry.h"


#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


int main(int argc, char* argv[]){
    renderer_init();
    RegisterSprite("smol","sprites/luna.png");
    while (!renderer_should_close()) {
        uint64_t frame_start = SDL_GetTicks();

        renderer_render(); 
        uint64_t frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    } 
    renderer_close();
    return 0;
}