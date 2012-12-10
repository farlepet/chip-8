#include <common.h>
#include <memory.h>
#include <SDL/SDL.h>
#include <math.h>

#ifndef GFX_H
#define GFX_H

#define WIDTH   320 //640
#define HEIGHT  200 //480
#define BPP     4
#define DEPTH   32

int init_gfx();

int gfx_upd();

SDL_Event get_input();

#endif