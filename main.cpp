//Project inspired from: https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

#include <iostream>
#include <SDL2/SDL.h>
#include "chip8.h"
using namespace std;


static void renderChip8(SDL_Renderer* renderer, SDL_Texture* texture, const unsigned char* gfx) {
    static unsigned int pixels[64 * 32];

    for (int i = 0; i < 64 * 32; i++) {
        pixels[i] = gfx[i] ? 0xFFFFFFFF : 0x000000FF;
    }

    SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(unsigned int));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}



int main()
{
    Chip8 emulator = Chip8();

    /*
        SDL Initialization and Graphics handeling
    */
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0){
        cerr << "SDL Initialization Failed" <<endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Chip8 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640, 480,
        0
    );

    if(!window){
        cerr << "Window Creation Failed" <<endl;
        return -2;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -3;
    }

    SDL_Texture* texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32
    );
    if (!texture) {
        std::cerr << "Texture creation failed: " << SDL_GetError() << "\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -4;
    }

    emulator.init();

    if (!emulator.loadROM("roms/Sierpinski.ch8")) {
        std::cerr << "Failed to load ROM\n";
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -5;
    }

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        emulator.nextCycle();

        if(emulator.shouldDraw()){
            renderChip8(renderer, texture, emulator.getGfx());
            emulator.clearDrawFlag();
        }

        SDL_Delay(0);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}