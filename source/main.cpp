/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2012 Peter Farley <far.peter1@gmail.com>
 * 
 */

#define USAGE "\
farcpu [file]\n\
  file: rom to use as main program\n\
ex: farcpu invaders.c8\n"

int ver[4] = {1,1,1,1};

#include <stdio.h>
#include <common.h>
#include <gfx.h>
#include <math.h>
#include <chip.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <time.h>

/* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)  *
 * 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F) *
 * 0x200-0xFFF - Program ROM and work RAM                       */



chip8 chip;

int main(int argc, char *argv[])
{
	
	printf("chip-8 v%d.%d.%d%c\n", ver[0], ver[1], ver[2], "ab "[ver[3]]);
	D("This is a build with DEBUG features included.\n");
	if(argc < 2){ printf(USAGE); exit(0); }
	if(argc > 2) chip.gfx_scale = atoi(argv[2]);
	
	chip.loadRom(argv[1]);
	chip.cycles = 0;

	bool skip = false;
	unsigned int i = 0;
	int spdup = 0;
	long int r;
	unsigned long int count = 0;
	clock_t start = clock();

	/* The following control expression uses short circuiting to make sure that
             clock() is only called every 1000 iterations. */
        //CLOCKS_PER_SEC
                                                                                
        printf("%ld clocks a second.\n", CLOCKS_PER_SEC);

	for(;;)
	{
		r = clock();
		
		//if(spdup == -1) skip = false;
		//if(spdup == 1)  skip = true;
		while(r + CLOCKS_PER_SEC/60 > clock())
		{
			//while((r + (CLOCKS_PER_SEC / 400)) > clock());
			//r = clock();
			//start = clock(); while(start + 1000 > clock());
			//r = clock(); 
			//for(r = 0; r < 0x1FFF; r++) 
			start = clock();
			while(start + 1 > clock()) 
			{ 
				for(r = 0; r < 0x2FF; r++) 
					spdup = chip.process_input(); 
				chip.cycle(); chip.cycles++;
				if((chip.cycles & 0xF) == 0xF) chip.gfx_upd(); 
			}
			
			
		}
		//chip.cycle();
		chip.timers();
	}

	printf("N cycles:%lld\n", chip.cycles);
	
	return 24;
}