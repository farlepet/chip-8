#include <gfx.h>

unsigned char gfx[64 * 32];



SDL_Surface *sdl_screen;
SDL_Event sdl_event;

void setscreenpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    Uint32 *pixmem32;
    Uint32 colour;  
 
    colour = SDL_MapRGB( screen->format, r, g, b );
  
    pixmem32 = (Uint32*) screen->pixels  + y*screen->w + x;
    *pixmem32 = colour;
}

void setpixel(int x, int y, u32int c)
{
	setscreenpixel(sdl_screen, x, y, (c&0xFF0000)>>15, (c&0xFF00)>>7, c&0xFF);
}

void UpdateScreen(SDL_Surface* screen)
{ 
    if(SDL_MUSTLOCK(screen)) 
    {
        if(SDL_LockSurface(screen) < 0) return;
    }

	//memcpy(screen->pixels, gfx_buff, screen->w * screen->h);
	int i = 0;
	for(i = 0; i < 64*32; i++)
	{
		if(gfx[i])setpixel(i, 0, 0x00FFFFFF);
		else      setpixel(i, 0, 0x00000000);
	}

    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
  
    SDL_Flip(screen); 
}

int gfx_upd()
{
	
	UpdateScreen(sdl_screen);
			                   
	return 0;
}

SDL_Event get_input()
{
	SDL_PollEvent(&sdl_event);
	return sdl_event;
}


int init_gfx()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ){ D("\nUnable to initilize SDL...\n\n"); exit(1); }
   
    if (!(sdl_screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE)))
    {
        SDL_Quit();
		D("\nUnable to initilize SDL screen...\n\n");
        exit(1);
    }
	/*
    while(!keypress) 
    {
         DrawScreen(screen,h++);
         while(SDL_PollEvent(&event)) 
         {      
              switch (event.type) 
              {
                  case SDL_QUIT:
	              keypress = 1;
	              break;
                  case SDL_KEYDOWN:
                       keypress = 1;
                       break;
              }
         }
    }
	*/
    //SDL_Quit();
  
    return 0;
}
