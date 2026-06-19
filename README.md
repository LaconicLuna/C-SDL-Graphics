Download stb_image from https://github.com/nothings/stb/blob/master/stb_image.h  
Extract SDL.zip  
To Compile:  
```
gcc main.c renderer.c sprite_registry.c -I SDL/include -L SDL/lib -lSDL3 -o game;
```
