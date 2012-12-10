#include <chip.h>

unsigned char chip8_fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

chip8::chip8()
{
	// empty
}

chip8::~chip8()
{
	// empty
}

void chip8::init()
{
	memset(memory, 0, 4096); //Clear the memory
	memset(V, 0, 16); //Set V* registers to 0
	memset(stack, 0, 16); //clear the stack
	memset(key, 0, 16); //Set all keys to 0
	memcpy(memory, chip8_fontset, 80); //copy fontset to memory;
	pc = 0x200;
	opcode = 0;
	I = 0;
	sp = 0;

	delay_timer = 0;
	sound_timer = 0;

	drawFlag = true;



	if (SDL_Init(SDL_INIT_VIDEO) < 0 ){ D("\nUnable to initilize SDL...\n\n"); exit(1); }
   
    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE)))
    {
        SDL_Quit();
		D("\nUnable to initilize SDL screen...\n\n");
        exit(1);
    }

	if(SDL_Init(SDL_INIT_AUDIO) != 0) { fprintf(stderr, "Warning: unable to initialize audio: %s\n", SDL_GetError()); }
}

void chip8::cycle()
{
	opcode = memory[pc] << 8 | memory[pc + 1];

	//D("%X::", opcode);

	// Process opcode
	switch(opcode & 0xF000)
	{		
		case 0x0000:
			switch(opcode & 0x000F)
			{
				case 0x0000: // 0x00E0: Clears the screen
					for(int i = 0; i < 2048; ++i)
						gfx[i] = 0x0;
					drawFlag = true;
					pc += 2;
				break;

				case 0x000E: // 0x00EE: Returns from subroutine
					--sp;			// 16 levels of stack, decrease stack pointer to prevent overwrite
					pc = stack[sp];	// Put the stored return address from the stack back into the program counter					
					pc += 2;		// Don't forget to increase the program counter!
				break;

				default:
					printf ("Unknown opcode [0x0000]: 0x%X\n", opcode);					
			}
		break;

		case 0x1000: // 0x1NNN: Jumps to address NNN
			pc = opcode & 0x0FFF;
		break;

		case 0x2000: // 0x2NNN: Calls subroutine at NNN.
			stack[sp] = pc;			// Store current address in stack
			++sp;					// Increment stack pointer
			pc = opcode & 0x0FFF;	// Set the program counter to the address at NNN
		break;
		
		case 0x3000: // 0x3XNN: Skips the next instruction if VX equals NN
			if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
		break;
		
		case 0x4000: // 0x4XNN: Skips the next instruction if VX doesn't equal NN
			if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
		break;
		
		case 0x5000: // 0x5XY0: Skips the next instruction if VX equals VY.
			if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
				pc += 4;
			else
				pc += 2;
		break;
		
		case 0x6000: // 0x6XNN: Sets VX to NN.
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			pc += 2;
		break;
		
		case 0x7000: // 0x7XNN: Adds NN to VX.
			V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
			pc += 2;
		break;
		
		case 0x8000:
			switch(opcode & 0x000F)
			{
				case 0x0000: // 0x8XY0: Sets VX to the value of VY
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0001: // 0x8XY1: Sets VX to "VX OR VY"
					V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0002: // 0x8XY2: Sets VX to "VX AND VY"
					V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0003: // 0x8XY3: Sets VX to "VX XOR VY"
					V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't					
					if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) 
						V[0xF] = 1; //carry
					else 
						V[0xF] = 0;					
					V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
					pc += 2;					
				break;

				case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
					if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) 
						V[0xF] = 0; // there is a borrow
					else 
						V[0xF] = 1;					
					V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0006: // 0x8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
					V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
					V[(opcode & 0x0F00) >> 8] >>= 1;
					pc += 2;
				break;

				case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
					if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])	// VY-VX
						V[0xF] = 0; // there is a borrow
					else
						V[0xF] = 1;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];				
					pc += 2;
				break;

				case 0x000E: // 0x8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
					V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
					V[(opcode & 0x0F00) >> 8] <<= 1;
					pc += 2;
				break;

				default:
					printf ("Unknown opcode [0x8000]: 0x%X\n", opcode);
			}
		break;
		
		case 0x9000: // 0x9XY0: Skips the next instruction if VX doesn't equal VY
			if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
				pc += 4;
			else
				pc += 2;
		break;

		case 0xA000: // ANNN: Sets I to the address NNN
			I = opcode & 0x0FFF;
			pc += 2;
		break;
		
		case 0xB000: // BNNN: Jumps to the address NNN plus V0
			pc = (opcode & 0x0FFF) + V[0];
		break;
		
		case 0xC000: // CXNN: Sets VX to a random number and NN
			V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
			pc += 2;
		break;
	
		case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
					 // Each row of 8 pixels is read as bit-coded starting from memory location I; 
					 // I value doesn't change after the execution of this instruction. 
					 // VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, 
					 // and to 0 if that doesn't happen
		{
			unsigned short x = V[(opcode & 0x0F00) >> 8];
			unsigned short y = V[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;
			unsigned short pixel;

			V[0xF] = 0;
			for (int yline = 0; yline < height; yline++)
			{
				pixel = memory[I + yline];
				for(int xline = 0; xline < 8; xline++)
				{
					if((pixel & (0x80 >> xline)) != 0)
					{
						if(gfx[(x + xline + ((y + yline) * 64))] == 1)
						{
							V[0xF] = 1;                                    
						}
						gfx[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}
						
			drawFlag = true;			
			pc += 2;
		}
		break;
			
		case 0xE000:
			switch(opcode & 0x00FF)
			{
				case 0x009E: // EX9E: Skips the next instruction if the key stored in VX is pressed
					if(key[V[(opcode & 0x0F00) >> 8]] != 0)
						pc += 4;
					else
						pc += 2;
				break;
				
				case 0x00A1: // EXA1: Skips the next instruction if the key stored in VX isn't pressed
					if(key[V[(opcode & 0x0F00) >> 8]] == 0)
						pc += 4;
					else
						pc += 2;
				break;

				default:
					printf ("Unknown opcode [0xE000]: 0x%X\n", opcode);
			}
		break;
		
		case 0xF000:
			switch(opcode & 0x00FF)
			{
				case 0x0007: // FX07: Sets VX to the value of the delay timer
					V[(opcode & 0x0F00) >> 8] = delay_timer;
					pc += 2;
				break;
								
				case 0x000A: // FX0A: A key press is awaited, and then stored in VX		
				{
					bool keyPress = false;

					for(int i = 0; i < 16; ++i)
					{
						if(key[i] != 0)
						{
							V[(opcode & 0x0F00) >> 8] = i;
							keyPress = true;
						}
					}

					// If we didn't received a keypress, skip this cycle and try again.
					if(!keyPress)						
						return;

					pc += 2;					
				}
				break;
				
				case 0x0015: // FX15: Sets the delay timer to VX
					delay_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
				break;

				case 0x0018: // FX18: Sets the sound timer to VX
					sound_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
				break;

				case 0x001E: // FX1E: Adds VX to I
					if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
						V[0xF] = 1;
					else
						V[0xF] = 0;
					I += V[(opcode & 0x0F00) >> 8];
					pc += 2;
				break;

				case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
					I = V[(opcode & 0x0F00) >> 8] * 0x5;
					pc += 2;
				break;

				case 0x0033: // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
					memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
					memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
					memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;					
					pc += 2;
				break;

				case 0x0055: // FX55: Stores V0 to VX in memory starting at address I					
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
						memory[I + i] = V[i];	

					// On the original interpreter, when the operation is done, I = I + X + 1.
					I += ((opcode & 0x0F00) >> 8) + 1;
					pc += 2;
				break;

				case 0x0065: // FX65: Fills V0 to VX with values from memory starting at address I					
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
						V[i] = memory[I + i];			

					// On the original interpreter, when the operation is done, I = I + X + 1.
					I += ((opcode & 0x0F00) >> 8) + 1;
					pc += 2;
				break;

				default:
					printf ("Unknown opcode [0xF000]: 0x%X\n", opcode);
			}
		break;

		default:
			printf ("Unknown opcode: 0x%X\n", opcode);
	}
	
	/*switch(opcode & 0xF000)
	{
		case 0x0000:
			switch(opcode & 0x000F)
			{
				case 0x0000: //0x00E0: clear screen
					memset(gfx, 0, 32*64); drawFlag = true; pc += 2; break;
				
				case 0x000E: //0x00EE: return from a subrutine
					sp--; pc = stack[sp]; pc += 2; break;

				default: //Unknown
					D("Unknown opcode: 0x%X\n", opcode); break;
			} break;
			
		case 0x1000: //0x1NNN: jump to NNN
			pc = opcode & 0x0FFF; break;

		case 0x2000: //0x2NNN: call a subrutine
			stack[sp] = pc; ++sp; pc = opcode & 0x0FFF; break;

		case 0x3000: //0x3XNN: skip next op if V[X] == NN
			if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) pc += 4; else pc += 2; break;

		case 0x4000: //0x4XNN: skip next op if V[X] != NN
			if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) pc += 4; else pc += 2; break;

		case 0x5000: //0x5XY0: skip next op if V[X] == V[Y]
			if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) pc += 4; else pc += 2; break;

		case 0x6000: //0x6XNN: set V[X] to NN
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF; pc += 2; break;

		case 0x7000: //0x7XNN: set V[X] to V[X] + NN
			V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF; pc += 2; break;

		case 0x8000:
			switch(opcode & 0x000F)
			{
				case 0x0000: //0x8XY0: sets V[X] to V[Y]
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4]; pc += 2; break;

				case 0x0001: //0x8XY1: Sets V[X] to V[X] | V[Y]
					V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4]; pc += 2; break;

				case 0x0002: //0x8XY2: Sets V[X] to V[X] & V[Y]
					V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4]; pc += 2; break;

				case 0x0003: //0x8XY3: Sets V[X] to V[X] ^ V[Y]
					V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4]; pc += 2; break;

				case 0x0004: //0x8XY4: Sets V[X] to V[X] + V[Y], V[F] == 1 means there was an overflow				
					if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) V[0xF] = 1;
					else V[0xF] = 0;					
					V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4]; pc += 2;

				case 0x0005:// 0x8XY5: Sets V[X] to V[X] - V[Y], V[F] == 0 means there was an underflow
					if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) V[0xF] = 0;
					else V[0xF] = 1;					
					V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4]; pc += 2;

				case 0x0006: //0x8X$6: V[X] >>= 1, V[F] = V[X] & 1
					V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1; V[(opcode & 0x0F00) >> 8] >>= 1; pc += 2; break;

				case 0x0007: //0x8XY7: Sets V[X] to V[Y] - V[X]. V[F] == 0 means there was an underflow
					if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) V[0xF] = 0;
					else V[0xF] = 1;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8]; pc += 2; break;

				case 0x000E: //0x8X$E: V[X] <<= 1, V[F] = V[X] >> 7
					V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7; V[(opcode & 0x0F00) >> 8] <<= 1; pc += 2; break;
				
				default:
					D("Unknown opcode: 0x%X\n", opcode); break;
			} break;
			

		case 0x9000: //0x9XY0: skip next op if V[X] != V[Y]
			if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) pc += 4; else pc += 2; break;
		
		case 0xA000: //0xANNN: sets I to NNN
			I = opcode & 0x0FFF; pc += 2; break;

		case 0xB000: //0xBNNN jumps to NNN + V[0]
			pc = (opcode & 0x0FFF) + V[0]; break;

		case 0xC000: //0xCXNN: Sets VX to rand & NN
			V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF); pc += 2; break;

		// DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
		// Each row of 8 pixels is read as bit-coded starting from memory location I; 
		// I value doesn't change after the execution of this instruction. 
		// VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, 
		// and to 0 if that doesn't happen
		case 0xD000: 
		{
			unsigned short x = V[(opcode & 0x0F00) >> 8];
			unsigned short y = V[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;
			unsigned short pixel;
			V[0xF] = 0;
			for (int yline = 0; yline < height; yline++){
				pixel = memory[I + yline];
				for(int xline = 0; xline < 8; xline++){
					if((pixel & (0x80 >> xline)) != 0){
						if(gfx[(x + xline + ((y + yline) * 64))] == 1) V[0xF] = 1;
						gfx[x + xline + ((y + yline) * 64)] ^= 1; 
					}
				}
			} drawFlag = true; pc += 2;
		} break; //Double brackets allow variable declarations

		case 0xE000:
			switch(opcode & 0x00FF)
			{
				case 0x009E: //0xEX9E: Skips next op if key in V[X] is pressed
					if(key[V[(opcode & 0x0F00) >> 8]] != 0) pc += 4; else pc += 2; break;
				
				case 0x00A1: //0xEXA1: Skips next op if key in V[X] isn't pressed
					if(key[V[(opcode & 0x0F00) >> 8]] == 0) pc += 4; else pc += 2; break;

				default:
					D("Unknown opcode: 0x%X\n", opcode); break;
			} break;

			
		case 0xF000:
			switch(opcode & 0x00FF)
			{
				case 0x0007: //0xFX07: Sets V[X] to delay timer
					V[(opcode & 0x0F00) >> 8] = delay_timer; pc += 2; break;

				case 0x000A: //0xFX0A: A key press is awaited, and then stored in VX		
				{
					bool keyPress = false;
					for(int i = 0; i < 16; ++i){
						if(key[i] != 0){
							V[(opcode & 0x0F00) >> 8] = i;
							keyPress = true;
						}
					}
					if(!keyPress) return; //skip cycle until key pressed
					pc += 2;					
				} break;

				case 0x0015: //0xFX15: Sets delay timer to V[X]
					delay_timer = V[(opcode & 0x0F00) >> 8]; pc += 2; break;

				case 0x0018: //0xFX18: Sets sound timer to V[X]
					sound_timer = V[(opcode & 0x0F00) >> 8]; pc += 2; break;

				case 0x001E: //0xFX1E: Adds VX to I
					if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF) V[0xF] = 1; //overflow
					else V[0xF] = 0;
					I += V[(opcode & 0x0F00) >> 8]; pc += 2; break;

				case 0x0029: //0xFX29: Sets I to location of sprite for char in V[X]. Chars 0x0-0xF are represented by a 4x5 font
					I = V[(opcode & 0x0F00) >> 8] * 0x5; pc += 2; break;

				case 0x0033: //0xFX33: Stores the Binary-coded decimal representation of V[X] at the addresses I, I + 1, and I + 2
					memory[I] = V[(opcode & 0x0F00) >> 8] / 100; memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
					memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10; pc += 2; break;

				case 0x0055: //0xFX55: Stores V[0] to V[X] in memory starting at address I					
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) memory[I + i] = V[i];	
					I += ((opcode & 0x0F00) >> 8) + 1; pc += 2; break;// On the original interpreter, when the operation is done, I = I + X + 1.
				
				case 0x0065: // FX65: Fills V[0] to V[X] with values from memory starting at address I					
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) V[i] = memory[I + i];			
					I += ((opcode & 0x0F00) >> 8) + 1; pc += 2; break;// On the original interpreter, when the operation is done, I = I + X + 1.


				default:
					D("Unknown opcode: 0x%X\n", opcode); break;
			} break;
			
		default:
			D("Unknown opcode: 0x%X\n", opcode); break;
	}*/

	

	if(delay_timer > 0) --delay_timer;
 
	if(sound_timer > 0)
	{
		if(sound_timer == 1) printf("BEEP!\n"); //TODO: inplement sound!
		--sound_timer;
	}  
}

void chip8::setpixel(int x, int y, u32int c)
{
	Uint32 *pixmem32;
    Uint32 colour;  
	x *= gfx_scale;
	y *= gfx_scale;
    colour = SDL_MapRGB( screen->format, (c&0xFF0000)>>15, (c&0xFF00)>>7, (c&0xFF));
  
    pixmem32 = (Uint32*) screen->pixels  + y*screen->w + x;
	int i, j;
	for(i = 0; i < gfx_scale; i++)
		for(j = 0; j < gfx_scale; j++)
			*(Uint32 *)((pixmem32) + i + j * screen->w) = colour;
	//setscreenpixel(sdl_screen, x, y, (c&0xFF0000)>>15, (c&0xFF00)>>7, c&0xFF);
}

void chip8::gfx_upd()
{
	if(SDL_MUSTLOCK(screen)) 
    {
        if(SDL_LockSurface(screen) < 0) return;
    }
	
	//memcpy(screen->pixels, gfx_buff, screen->w * screen->h);
	if(drawFlag)
	{
		int i = 0, j;
		for(; i < 64; i++)
		{
			for(j = 0; j < 32; j++)
			{
				if(gfx[i+j*64]) setpixel(i, j, 0x00FFFFFF);
				else            setpixel(i, j, 0x00000000);
			}
		}
		drawFlag = false;
	}
	
    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
  
    SDL_Flip(screen); 
}

SDL_Event chip8::get_input()
{
	SDL_PollEvent(&event);
	return event;
}

void chip8::loadRom(const char * filename)
{
	init();
	printf("Loading: %s\n", filename);
		
	FILE *rom;
	if ((rom = fopen(filename, "rb")) == NULL) { fputs("File error", stderr); exit(0); }

	// Check file size
	fseek(rom , 0 , SEEK_END);
	long lSize = ftell(rom);
	rewind(rom);
	printf("Filesize: %d\n", (int)lSize);

	// Allocate memory to contain the whole file
	char * buffer = (char*)malloc(sizeof(char) * lSize);
	if (buffer == NULL)  { fputs ("Memory error", stderr); exit(0); }

	// Copy the file into the buffer
	size_t result = fread (buffer, 1, lSize, rom);
	if (result != lSize)  { fputs("Reading error", stderr);  exit(0); }

	// Copy buffer to Chip8 memory
	if((4096-512) > lSize) 
		for(int i = 0; i < lSize; ++i) memory[i + 512] = buffer[i];
	else
		printf("Error: ROM too big for memory");
	
	// Close file, free buffer
	fclose(rom);
	free(buffer);

	return;
}

int chip8::process_input()
{
	while (SDL_PollEvent(&event))   //Poll our SDL key event for any keystrokes.
	{
		switch(event.type){
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym){
					case SDLK_1: key[0x1] = 1; break;
					case SDLK_2: key[0x2] = 1; break;
					case SDLK_3: key[0x3] = 1; break;
					case SDLK_4: key[0xC] = 1; break;

					case SDLK_q: key[0x4] = 1; break;
					case SDLK_w: key[0x5] = 1; break;
					case SDLK_e: key[0x6] = 1; break;
					case SDLK_r: key[0xD] = 1; break;

					case SDLK_a: key[0x7] = 1; break;
					case SDLK_s: key[0x8] = 1; break;
					case SDLK_d: key[0x9] = 1; break;
					case SDLK_f: key[0xE] = 1; break;

					case SDLK_z: key[0xA] = 1; break;
					case SDLK_x: key[0x0] = 1; break;
					case SDLK_c: key[0xB] = 1; break;
					case SDLK_v: key[0xF] = 1; break;


					case SDLK_g: return 1;

					default: break;
				} break;

			case SDL_KEYUP:
				switch(event.key.keysym.sym){
					case SDLK_1: key[0x1] = 0; break;
					case SDLK_2: key[0x2] = 0; break;
					case SDLK_3: key[0x3] = 0; break;
					case SDLK_4: key[0xC] = 0; break;

					case SDLK_q: key[0x4] = 0; break;
					case SDLK_w: key[0x5] = 0; break;
					case SDLK_e: key[0x6] = 0; break;
					case SDLK_r: key[0xD] = 0; break;

					case SDLK_a: key[0x7] = 0; break;
					case SDLK_s: key[0x8] = 0; break;
					case SDLK_d: key[0x9] = 0; break;
					case SDLK_f: key[0xE] = 0; break;

					case SDLK_z: key[0xA] = 0; break;
					case SDLK_x: key[0x0] = 0; break;
					case SDLK_c: key[0xB] = 0; break;
					case SDLK_v: key[0xF] = 0; break;

					case SDLK_g: return -1;

					default: break;
				} break;

			case SDL_QUIT: exit(0);
		}
	}

	return 0;
}