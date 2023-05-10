#include <cstdio>
#include <string.h>
#include <fstream>
#include "chip8.h"

// font set of 16 characters at 5 bytes each for rendering
// 16 * 5 = 80 bytes size fontset
// Each number or character is 4 pixels wide and 5 pixel high.
const unsigned char fontset[FONTSET_SIZE] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

chip8::chip8()
{
    // Initialize PC
	pc = START_ADDRESS;

    // Load fonts into memory
	for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}
}

// function that loads the contents of a ROM file
void chip8::LoadROM(char const* filename)
{
	// Open the file as a stream of binary and move the file pointer to the end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;
	}
}

void chip8::emulateCycle()
{
    // fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    // decode opcodes
    switch (opcode & 0xF000)
    {
        case 0x0000:
            switch (opcode & 0X00F)
            {
                case 0x0000:    // 0x00E0: Clears the screen
                    // execute opcode
                break;
            
                case 0X000E:    // 0x00EE: Returns from subroutine 
                    // execute opcode
                break;
        
                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }                
        break;

        // more opcodes

        case 0X2000:    // 0x2NNN: Calls subroutine at NNN.
            stack[sp] = pc;        // Store current address in stack
            ++sp;                    // Increment stack pointer
            pc = opcode & 0x0FFF;    // Set the program counter to the address at NNN
        break;
    }

    // update timers
    if (delay_timer > 0)    
        --delay_timer;

    if (sound_timer > 0)
    {
        if (sound_timer == 1)
            printf("BOOM\n");
        --sound_timer;
    }
}

