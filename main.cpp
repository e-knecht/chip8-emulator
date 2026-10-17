#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <iostream>
#include "chip8.hpp"

int main(int argc, char* argv[]) {
    // Start SDL's video system
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    Chip8 chip;
    bool ok = chip.loadROM("roms/2-ibm-logo.ch8");

    std::cout << "loaded: " << ok << "\n";
    std::cout << std::hex << (int)chip.memory[0x200] << " "
              << (int)chip.memory[0x201] << "\n";

    // CHIP-8's screen is 64x32, so a 640x320 window is 10x scaled up
    SDL_Window* window = SDL_CreateWindow(
        "CHIP-8",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 320, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window failed: " << SDL_GetError() << "\n";
        return 1;
    }

    // The renderer is what actually draws into the window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    bool running = true;
    SDL_Event event;
    while (running) {
        for(int i = 0; i < 10; ++i){
            chip.cycle();
        }
        // Check for events (closing the window, key presses, etc.)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
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
        SDL_RenderPresent(renderer);                    // show it
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
