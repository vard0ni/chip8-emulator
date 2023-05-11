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
    : randomEngine(std::chrono::system_clock::now().time_since_epoch().count())
{
    // Initialize PC
	pc = START_ADDRESS;

    // Load fonts into memory
	for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

    // Initialize RNG
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

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

// Clear the display. Set the entire video buffer to zeros.
void chip8::OP_00E0()
{
    memset(display, 0, sizeof(display));
}

// Return from a subroutine. The top of the stack has the address of one
// instruction past the one that called the subroutine, so we can put that back
// into the PC. Note that this overwrites our preemptive pc += 2 earlier.
void chip8::OP_00EE()
{
    --sp;
    pc = stack[sp];
}

// Jump to location nnn. The interpreter sets program counter to nnn.
// A jump doesn’t remember its origin, so no stack interaction required.
void chip8::OP_1nnn()
{
    uint16_t address = opcode & 0x0FFFu;

    pc = address;
}

// Call subroutine at nnn. 
void chip8::OP_2nnn()
{
    uint16_t address = opcode & 0x0FFFu;

    stack[sp] = pc;
    ++sp;
    pc = address;
}

// Skip next instruction if Vx == kk.
void chip8::OP_3xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] == byte)
    {
        pc += 2;
    }
}

// Skip next instruction if Vx != kk.
void chip8::OP_4xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] != byte)
    {
        pc += 2;
    }
}

// Skip next instruction if Vx = Vy.
void chip8::OP_5xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;


    if (registers[Vx] == registers[Vy])
	{
		pc += 2;
	}
}

// Set Vx = kk
void chip8::OP_6xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = byte;
}

// Set Vx = Vx + kk.
void chip8::OP_7xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] += byte;
}

// Set Vx = Vy
void chip8::OP_8xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vy];
}

// Set Vx = Vx OR Vy
void chip8::OP_8xy1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] |= registers[Vy];
}

// Set Vx = Vx AND Vy.
void chip8::OP_8xy2()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] &= registers[Vy];
}

// Set Vx = Vx XOR Vy
void chip8::OP_8xy3()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] ^= registers[Vy];
}

// Set Vx = Vx + Vy, set VF = carry
// Values of Vx and Vy added together. If result greater than 8 bits (i.e., > 255,)
// VF is set to 1, otherwise 0. Only lowest 8 bits of result are kept, and stored in Vx.
void chip8::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t sum = registers[Vx] + registers[Vy];

	if (sum > 255U)
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = sum & 0xFFu;
}

// Set Vx = Vx - Vy, set VF = NOT borrow
// If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx,
// and the results stored in Vx.
void chip8::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] -= registers[Vy];
}

// Set Vx = Vx SHR 1
// If least-significant bit of Vx = 1, then VF is set to 1, otherwise 0. Then Vx divided by 2.
// Right shift performed (division by 2), and the least significant bit saved in Register VF.
void chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save LSB in VF
	registers[0xF] = (registers[Vx] & 0x1u);

	registers[Vx] >>= 1;
}

// Set Vx = Vy - Vx, set VF = NOT borrow.
// If Vy > Vx -> VF set = 1, otherwise 0. Then Vx subtracted from Vy, and results stored in Vx
void chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vy] - registers[Vx];
}

// Set Vx = Vx SHL 1.
// If most-significant bit of Vx = 1, then VF set to 1, otherwise to 0. Then Vx multiplied by 2.
// Left shift performed (multiplication by 2), and most significant bit is saved in Register VF.
void chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save MSB in VF
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;
}

// Skip next instruction if Vx != Vy
// Since our PC increment by 2 in Cycle(), we can increment by 2 again to skip next instruction
void chip8::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] != registers[Vy])
	{
		pc += 2;
	}
}

// Set I = nnn
void chip8::OP_Annn()
{
	uint16_t address = opcode & 0x0FFFu;

	index = address;
}

// Jump to location nnn + V0
void chip8::OP_Bnnn()
{
	uint16_t address = opcode & 0x0FFFu;

	pc = registers[0] + address;
}

// Set Vx = random byte AND kk
void chip8::OP_Cxkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = randByte(randomEngine) & byte;
}

// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
void chip8::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap if going beyond screen boundaries
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

	registers[0xF] = 0;

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[index + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &display[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

// Skip next instruction if key with the value of Vx is pressed.
// Since our PC incremented by 2 in Cycle(), we can increment by 2 again to skip next instruction
void chip8::OP_Ex9E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

	if (keypad[key])
	{
		pc += 2;
	}
}

// Skip next instruction if key with the value of Vx is not pressed
void chip8::OP_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

	if (!keypad[key])
	{
		pc += 2;
	}
}

// Set Vx = delay timer value
void chip8::OP_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[Vx] = delay_timer;
}

// Wait for a key press, store the value of the key in Vx.
// The easiest way to “wait” is to decrement the PC by 2 whenever a keypad value is not detected.
// This has the effect of running the same instruction repeatedly.
void chip8::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (keypad[0])
	{
		registers[Vx] = 0;
	}
	else if (keypad[1])
	{
		registers[Vx] = 1;
	}
	else if (keypad[2])
	{
		registers[Vx] = 2;
	}
	else if (keypad[3])
	{
		registers[Vx] = 3;
	}
	else if (keypad[4])
	{
		registers[Vx] = 4;
	}
	else if (keypad[5])
	{
		registers[Vx] = 5;
	}
	else if (keypad[6])
	{
		registers[Vx] = 6;
	}
	else if (keypad[7])
	{
		registers[Vx] = 7;
	}
	else if (keypad[8])
	{
		registers[Vx] = 8;
	}
	else if (keypad[9])
	{
		registers[Vx] = 9;
	}
	else if (keypad[10])
	{
		registers[Vx] = 10;
	}
	else if (keypad[11])
	{
		registers[Vx] = 11;
	}
	else if (keypad[12])
	{
		registers[Vx] = 12;
	}
	else if (keypad[13])
	{
		registers[Vx] = 13;
	}
	else if (keypad[14])
	{
		registers[Vx] = 14;
	}
	else if (keypad[15])
	{
		registers[Vx] = 15;
	}
	else
	{
		pc -= 2;
	}
}

// Set delay timer = Vx
void chip8::OP_Fx15()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	delay_timer = registers[Vx];
}

// Set sound timer = Vx
void chip8::OP_Fx18()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	sound_timer = registers[Vx];
}

// Set I = I + Vx
void chip8::OP_Fx1E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	index += registers[Vx];
}

// Set I = location of sprite for digit Vx
// We know font characters are located at 0x50, and we know they’re five bytes each,
// so we can get address of first byte any character by taking an offset from start address.
void chip8::OP_Fx29()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = registers[Vx];

	index = FONTSET_START_ADDRESS + (5 * digit);
}

// Store BCD(binary) representation of Vx in memory locations I, I+1, and I+2
// The interpreter takes the decimal value of Vx, and places hundreds digit in memory
// at location in I, the tens digit at location I+1, and the ones digit at location I+2.
void chip8::OP_Fx33()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];

	// Ones-place
	memory[index + 2] = value % 10;
	value /= 10;

	// Tens-place
	memory[index + 1] = value % 10;
	value /= 10;

	// Hundreds-place
	memory[index] = value % 10;
}

// Store registers V0 through Vx in memory starting at location I.
void chip8::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		memory[index + i] = registers[i];
	}
}

// Read registers V0 through Vx from memory starting at location I.
void chip8::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		registers[i] = memory[index + i];
	}
}