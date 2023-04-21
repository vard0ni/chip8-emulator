#include <string.h>
#include "chip8.h"

// font set of 16 characters at 5 bytes each for rendering
// 16 * 5 = 80 bytes size fontset
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

}

chip8::~chip8()
{

}

void chip8::init()
{
    pctr    = 0x200;    // program counter starts at 0x200
    opcode  = 0;        // reset current opcode
    I       = 0;        // reset index register
    sptr    = 0;        // reset stack pointer

    int i;

    // clear display
    for (i = 0; i < DISPLAY_SIZE; ++i)
        display[i] = 0;

    // clear stack
    for (i = 0; i < STACK_SIZE; ++i)
        stack[i] = 0;

    // clear registers
    for (i = 0; i < NUM_REGISTERS; ++i)
        V[i] = 0;

    // clear keypad
    for (i = 0; i < NUM_KEYS; ++i)
        key[i] = 0;

    // clear memory
    for (i = 0; i < SYSTEM_MEMORY; ++i)
        memory[i] = 0;

    // load fontset
    for (i = 0; i < FONTSET_SIZE; ++i)
        memory[i] = fontset[i];

    // reset timers
    delay_timer = 0;
    sound_timer = 0;
}

void chip8::emulateCycle()
{
    // fetch opcode

    // execute opcode

    //
}

