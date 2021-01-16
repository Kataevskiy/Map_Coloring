#include <SDL.h>
#include <iostream>
#include "imagesaver.h"

using namespace std;

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *mainWindow = SDL_CreateWindow("Map_Coloring", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1200, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer *mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(mainRenderer, 255, 255, 255, 255);
    SDL_RenderClear(mainRenderer);
    SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 255);

    SDL_Event e;
    bool quit = false;
    bool drawing = false;
    int lastX, lastY;
    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                quit = true;
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (SDL_GetMouseState(&lastX, &lastY) & SDL_BUTTON(SDL_BUTTON_LEFT))
                    drawing = true;
            }
            else if (e.type == SDL_MOUSEBUTTONUP)
            {
                if (!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
                    drawing = false;
            }
        }

        if (drawing)
        {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if (lastX != x || lastY != y)
            {
                SDL_RenderDrawLine(mainRenderer, lastX, lastY, x, y);
                lastX = x;
                lastY = y;
            }
        }

        SDL_RenderPresent(mainRenderer);
    }

    return 1;
}