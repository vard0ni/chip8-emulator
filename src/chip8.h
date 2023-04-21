#pragma once

#define SYSTEM_MEMORY   4096
#define DISPLAY_SIZE    2048
#define FONTSET_SIZE    80
#define STACK_SIZE      16
#define NUM_REGISTERS   16
#define NUM_KEYS        16

class chip8
{
private:
    unsigned short opcode;                // current opcode
    unsigned short I;                     // index register for draw
    unsigned short pctr;                  // program counter
    unsigned short sptr;                  // stack pointer

    unsigned char memory[SYSTEM_MEMORY];  // Memory(size = 4k)
    unsigned char V[NUM_REGISTERS];       // V-registers(V0..VF)    
    unsigned short stack[STACK_SIZE];     // Stack(16 16-bit values)

    

    unsigned char delay_timer;
    unsigned char sound_timer;

    void init();

public:
    chip8();
    ~chip8();

    unsigned char display[DISPLAY_SIZE];
    unsigned char key[NUM_KEYS];          // hex based keypad(0x0-0xF)

    void emulateCycle();

};