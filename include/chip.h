#include <common.h>
#include <SDL/SDL.h>
#include <audio.h>

#ifndef CHIP_H
#define CHIP_H

#define WIDTH   64*gfx_scale
#define HEIGHT  32*gfx_scale
#define BPP     4
#define DEPTH   32

class chip8 {
	public:
		chip8();
		~chip8();
		
		bool drawFlag;

		void cycle();
		int process_input();
		void gfx_upd();
		void loadRom(const char * filename);		

		unsigned char  gfx_scale = 2;
		unsigned char  gfx[64 * 32];	// Total amount of pixels: 2048
		unsigned char  key[16];		

		void beep();

	private:	
		unsigned short pc;				// Program counter
		unsigned short opcode;			// Current opcode
		unsigned short I;				// Index register
		unsigned short sp;				// Stack pointer
		
		unsigned char  V[16];			// V-regs (V0-VF)
		unsigned short stack[16];		// Stack (16 levels)
		unsigned char  memory[4096];	// Memory (size = 4k)		
				
		unsigned char  delay_timer;		// Delay timer
		unsigned char  sound_timer;		// Sound timer	

		Beeper beepr;
		
		SDL_Surface *screen;
		SDL_Event event;
		

		SDL_Event get_input();
		
		void setpixel(int x, int y, u32int c);

		void init();
};

#endif