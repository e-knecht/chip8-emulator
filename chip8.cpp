#include "chip8.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>

Chip8::Chip8() : memory{}, reg{}, index{0}, pc{0x200}, st{}, sp{0},
delay{0}, sound{0}, display{}, keypad{} { }

bool Chip8::loadROM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if(!file){
        return false;
    }
    std::streamsize filesize = file.tellg();
    if(filesize > 4096 - 0x200){
        return false;
    }
    file.seekg(0);
    file.read(reinterpret_cast<char*>(&memory[0x200]), filesize);
    return static_cast<bool>(file);
}

void Chip8::cycle(){
    uint16_t opcode = memory[pc]<<8 | memory[pc + 1];
    pc += 2;

    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = opcode & 0x000F;
    uint8_t nn = opcode & 0x00FF;
    uint16_t nnn = opcode & 0x0FFF;

    switch (opcode>>12){
        case 0x0:
            switch (opcode & 0x00FF){
                case 0xE0:
                    std::fill(std::begin(display), std::end(display), 0);
                    break;
                case 0xEE:
                            //return from a subroutine
                    break;
                default:
                    std::cout << "unknown opcode: " << std::hex << opcode << "\n";
                    break;
            }
            break;
        case 0x1:
            pc = nnn;
            break;
        case 0x2:
            break;
        case 0x3:
            break;
        case 0x4:
            break;
        case 0x5:
            break;
        case 0x6:
            reg[x] = nn;
            break;
        case 0x7:
            reg[x] += nn;
            break;
        case 0x8:
            break;
        case 0x9:
            break;
        case 0xA:
            index = nnn;
            break;
        case 0xB:
            break;
        case 0xC:
            break;
        case 0xD: {
            uint8_t xpos = reg[x] % 64;
            uint8_t ypos = reg[y] % 32;
            reg[0xF] = 0;
            for(int row = 0; row < n; ++row){
                uint8_t spriteByte = memory[index + row];
                for(int col = 0; col < 8; ++col){
                    if((spriteByte & (0x80 >> col)) != 0){
                        int screenX = xpos + col;
                        int screenY = ypos + row;
                        if(screenX >= 64 || screenY >= 32){
                            continue;
                        }
                        int pos = screenY * 64 + screenX;
                        if(display[pos]){
                            reg[0xF] = 1;
                        }
                        display[pos] ^= 1;
                    }
                }
            }
            break;
        }
        case 0xE:
            break;
        case 0xF:
            break;
        default:
            std::cout << "unknown opcode: " << std::hex << opcode << "\n";
            break;
    }
}
