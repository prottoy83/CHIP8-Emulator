//Project inspired from: https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

#include <iostream>
#include <SDL2/SDL.h>
#include "chip8.h"
using namespace std;


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

    emulator.init();

    bool emulatorAlive = true;
    while(emulatorAlive){
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT)
                emulatorAlive = false;
        }

        emulator.nextCycle();

        SDL_Delay(1);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}