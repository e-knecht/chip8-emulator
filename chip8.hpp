#pragma once
#include <cstdint>
#include <string>

class Chip8 {
    public:
        Chip8();
        uint8_t memory[4096];   //4096 bytes of RAM
        uint8_t reg[16];    //16, 1 byte registers
        uint16_t index;
        uint16_t pc;
        uint16_t st[16];
        uint8_t sp;     //index of stack
        uint8_t delay;
        uint8_t sound;
        uint8_t display[64 * 32];   //row 0, 0-63 and so on
        uint8_t keypad[16];
        bool frame;
        bool reported[0x10000];
        bool pressed;
        uint8_t key;

        bool loadROM(const std::string& filename);

        void cycle();

        void tickTimers();
};
