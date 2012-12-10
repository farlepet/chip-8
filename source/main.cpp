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

int ver[4] = {0,1,0,0};

#include <stdio.h>
#include <common.h>
#include <gfx.h>
#include <math.h>
#include <chip.h>
#include <unistd.h>

/* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)  *
 * 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F) *
 * 0x200-0xFFF - Program ROM and work RAM                       */



chip8 chip;

int main(int argc, char *argv[])
{
	printf("chip-8 v%d.%d.%d%c\n", ver[0], ver[1], ver[2], "ab "[ver[3]]);
	D("This is a build with DEBUG features included.\n");
	if(argc < 2){ printf(USAGE); exit(0); }
	if(argc == 3) chip.gfx_scale = atoi(argv[2]); 
	
	chip.loadRom(argv[1]);

	int i = 0;

	for(;;)
	{
		for(i = 0; i < 100; i++)
			chip.process_input();
		chip.cycle();
		chip.gfx_upd();\
	}
	
	return 24;
}