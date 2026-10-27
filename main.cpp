#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <iostream>
#include "chip8.hpp"
#include <cstdlib>
#include <ctime>

//--TUNE PER ROM--
const int CYCLES_PER_FRAME = 10; //  *60 fps = operations per second

// Return slot in keypad[] for keycode
int keyToSlot(SDL_Keycode key) {
    switch(key) {
        case SDLK_1:
            return 0x1;
        case SDLK_2:
            return 0x2;
        case SDLK_3:
            return 0x3;
        case SDLK_4:
            return 0xC;
        case SDLK_q:
            return 0x4;
        case SDLK_w:
            return 0x5;
        case SDLK_e:
            return 0x6;
        case SDLK_r:
            return 0xD;
        case SDLK_a:
            return 0x7;
        case SDLK_s:
            return 0x8;
        case SDLK_d:
            return 0x9;
        case SDLK_f:
            return 0xE;
        case SDLK_z:
            return 0xA;
        case SDLK_x:
            return 0x0;
        case SDLK_c:
            return 0xB;
        case SDLK_v:
            return 0xF;
        default:
            return -1;
    }
}

int main(int argc, char* argv[]) {
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <rom-path>\n";
        return 1;
    }
    Chip8 chip;
    if (!chip.loadROM(argv[1])){
        std::cerr << "Failed to load ROM\n";
        return 1;
    }

    // Start SDL's video system
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    srand(time(nullptr));

    // CHIP-8's screen is 64x32, so a 640x320 window is 10x scaled up
    SDL_Window* window = SDL_CreateWindow(
        argv[1],
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 320, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    // The renderer is what actually draws into the window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        std::cerr << "Renderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;
    while (running) {
        for(int i = 0; i < CYCLES_PER_FRAME; ++i){
            chip.cycle();
        }
        // Check for events (closing the window, key presses, etc.)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if(event.type == SDL_KEYDOWN){
                int slot = keyToSlot(event.key.keysym.sym);
                if (slot >= 0){
                    chip.keypad[slot] = 1;
                }
            }
            if(event.type == SDL_KEYUP){
                int slot = keyToSlot(event.key.keysym.sym);
                if (slot >= 0){
                    chip.keypad[slot] = 0;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black
        SDL_RenderClear(renderer);                      // fill the screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // white for "on" pixels
        for(int i = 0; i < 64 * 32; ++i){
            int col = i % 64;
            int row = i / 64;
            if(chip.display[i] != 0){
                SDL_Rect rect = { col * 10, row * 10, 10, 10};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
        SDL_RenderPresent(renderer);
        chip.tickTimers();
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
