#include "chip8.hpp"
#include <fstream>
#include <string>

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
