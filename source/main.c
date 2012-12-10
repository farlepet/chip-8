/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2012 Peter Farley <far.peter1@gmail.com>
 * 
 */

int ver[4] = {0,0,1,0};

#include <stdio.h>
#include <common.h>
#include <gfx.h>
#include <math.h>

unsigned char memory[4096];
/* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)  *
 * 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F) *
 * 0x200-0xFFF - Program ROM and work RAM                       */



unsigned short opcode;
unsigned char memory[4096];
unsigned char V[16];
unsigned short I;
unsigned short pc;
unsigned char delay_timer, sound_timer;

int main(int argc, char *argv[])
{
	

	printf("chip-8 v%d.%d.%d%c\n", ver[0], ver[1], ver[2], "ab "[ver[3]]);
	D("This is a build with DEBUG features included.\n");

	init_gfx();
	
	return 24;
}