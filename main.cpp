#include <SDL.h>
#include "imagesaver.h"

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *mainWindow = SDL_CreateWindow("Map_Coloring", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1200, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer *mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Delay(3000);
    return 1;
}