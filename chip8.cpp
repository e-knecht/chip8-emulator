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
                    if(sp == 0){
                        std::cerr << "stack underflow\n";
                        break;
                    }
                    pc = st[--sp];
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
            if(sp >= 16){
                std::cerr << "stack overflow\n";
                break;
            }
            st[sp] = pc;
            sp++;
            pc = nnn;
            break;
        case 0x3:
            if(reg[x] == nn){
                pc += 2;
            }
            break;
        case 0x4:
            if(reg[x] != nn){
                pc += 2;
            }
            break;
        case 0x5:
            if(reg[x] == reg[y]){
                pc += 2;
            }
            break;
        case 0x6:
            reg[x] = nn;
            break;
        case 0x7:
            reg[x] += nn;
            break;
        case 0x8:
            switch (n){
                case 0:
                    reg[x] = reg[y];
                    break;
                case 1:
                    reg[x] |= reg[y];
                    break;
                case 2:
                    reg[x] &= reg[y];
                    break;
                case 3:
                    reg[x] ^= reg[y];
                    break;
                case 4:{
                    int sum = reg[x] + reg[y];
                    reg[x] += reg[y];
                    if (sum > 255){
                        reg[0xF] = 1;
                    }
                    else{
                        reg[0xF] = 0;
                    }
                    break;
                }
                case 5: {
                    int sub = reg[x] - reg[y];
                    reg[x] -= reg[y];
                    if(sub < 0){
                        reg[0xF] = 0;
                    }
                    else{
                        reg[0xF] = 1;
                    }
                    break;
                }
                case 6: {
                    int bit = reg[x] & 0x1;
                    reg[x] = reg[x] >> 1;
                    reg[0xF] = bit;
                    break;
                }
                case 7: {
                    int sub = reg[y] - reg[x];
                    reg[x] = reg[y] - reg[x];
                    if(sub < 0){
                        reg[0xF] = 0;
                    }
                    else{
                        reg[0xF] = 1;
                    }
                    break;
                }
                case 0xE: {
                    int bit = (reg[x] & 0x80) >> 7;
                    reg[x] = reg[x] << 1;
                    reg[0xF] = bit;
                    break;
                }
                default:
                    std::cout << "unknown opcode: " << std::hex << opcode << "\n";
                    break;
            }
            break;
        case 0x9:
            if(reg[x] != reg[y]){
                pc += 2;
            }
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
            switch (nn){
                case 0x1E:
                    index += reg[x];
                    break;
                case 0x33: {
                    int dec = reg[x];
                    memory[index + 2] = dec % 10;
                    memory[index + 1] = (dec / 10) % 10;
                    memory[index] = (dec / 100) % 10;
                    break;
                }
                case 0x55: {
                    for(int i = 0; i <= x; ++i){
                        memory[index + i] = reg[i];
                    }
                    break;
                }
                case 0x65: {
                    for(int i = 0; i <= x; ++i){
                        reg[i] = memory[index + i];
                    }
                    break;
                }
                default:
                    std::cout << "unknown opcode: " << std::hex << opcode << "\n";
                    break;
            }
            break;
        default:
            std::cout << "unknown opcode: " << std::hex << opcode << "\n";
            break;
    }
}
