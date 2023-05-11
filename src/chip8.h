#pragma once

#include <cstdint>
#include <cstdio>
#include <string.h>
#include <fstream>
#include <chrono>
#include <random>

#define SYSTEM_MEMORY   4096
#define VIDEO_WIDTH     64
#define VIDEO_HEIGHT    32
#define DISPLAY_SIZE    VIDEO_WIDTH * VIDEO_HEIGHT
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

    std::default_random_engine randomEngine;    // generate random numbers
    std::uniform_int_distribution<uint8_t> randByte; // random between 2 nums

    // CLS
	void OP_00E0();

    // RET
    void OP_00EE();

    // JUMP address
    void OP_1nnn();

    // CALL address
    void OP_2nnn();

    // SE Vx, byte
    void OP_3xkk();

    // SNE Vx, byte
    void OP_4xkk();

    // SE Vx, Vy
	void OP_5xy0();

    // LD Vx, byte
	void OP_6xkk();

    // ADD Vx, byte
	void OP_7xkk();

    // LD Vx, Vy
	void OP_8xy0();

    // OR Vx, Vy
	void OP_8xy1();

    // AND Vx, Vy
	void OP_8xy2();

    // XOR Vx, Vy
	void OP_8xy3();

    // ADD Vx, Vy
	void OP_8xy4();

    // SUB Vx, Vy
	void OP_8xy5();

    // SHR Vx
	void OP_8xy6();

    // SUBN Vx, Vy
	void OP_8xy7();

    // SHL Vx
	void OP_8xyE();

    // SNE Vx, Vy
	void OP_9xy0();

    // LD I, address
	void OP_Annn();

    // JP V0, address
	void OP_Bnnn();

    // RND Vx, byte
	void OP_Cxkk();

    // DRW Vx, Vy, height
	void OP_Dxyn();

    // SKP Vx
	void OP_Ex9E();

    // SKNP Vx
	void OP_ExA1();

    // LD Vx, DT
	void OP_Fx07();

    // LD Vx, K
	void OP_Fx0A(); 

    // LD DT, Vx
	void OP_Fx15();

    // LD ST, Vx
	void OP_Fx18();

    // ADD I, Vx
	void OP_Fx1E();

    // LD F, Vx
	void OP_Fx29();

    // LD B, Vx
	void OP_Fx33();

    // LD [I], Vx
	void OP_Fx55();

    // LD Vx, [I]
	void OP_Fx65();

public:
    chip8();

    void LoadROM(char const* filename);
    void emulateCycle();

    uint32_t display[DISPLAY_SIZE];
    uint8_t keypad[NUM_KEYS];              // hex based keypad(0x0-0xF)

    
};