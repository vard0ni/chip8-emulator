#pragma once

#include <cstdint>

#define SYSTEM_MEMORY   4096
#define DISPLAY_SIZE    2048
#define FONTSET_SIZE    80
#define STACK_SIZE      16
#define NUM_REGISTERS   16
#define NUM_KEYS        16

const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;

class chip8
{
private:
    uint16_t opcode;                    // current opcode
    uint16_t index;                     // index register for draw
    uint16_t pc;                        // program counter
    uint8_t sp;                         // stack pointer

    uint8_t memory[SYSTEM_MEMORY];      // Memory(size = 4k)
    uint8_t registers[NUM_REGISTERS];   // V-registers(V0..VF)    
    uint16_t stack[STACK_SIZE];         // Stack(16 16-bit values)


    uint8_t delay_timer;
    uint8_t sound_timer;

public:
    chip8();

    void LoadROM(char const* filename);
    void emulateCycle();

    uint32_t display[DISPLAY_SIZE];
    uint8_t key[NUM_KEYS];              // hex based keypad(0x0-0xF)

    
};