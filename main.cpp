#include <SDL.h>
#include <iostream>
#include "imagesaver.h"

using namespace std;

const int width = 1600, height = 900;

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *mainWindow = SDL_CreateWindow("Map_Coloring", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
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
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_RETURN)
                {
                    SDL_Surface *tempSurface = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
                    SDL_RenderReadPixels(mainRenderer, NULL, SDL_PIXELFORMAT_RGBA32, tempSurface->pixels, tempSurface->pitch);
                    imageRGBA map(width, height);
                    for (int i = 0; i < height; i++)
                        for (int j = 0; j < width; j++)
                        {
                            pixelRGBA pixel;
                            pixel.r = (Uint8)(((Uint32 *)tempSurface->pixels)[i * width + j] >> 16);
                            pixel.g = (Uint8)(((Uint32 *)tempSurface->pixels)[i * width + j] >> 8);
                            pixel.b = (Uint8)(((Uint32 *)tempSurface->pixels)[i * width + j] >> 0);
                            pixel.a = (Uint8)(((Uint32 *)tempSurface->pixels)[i * width + j] >> 24);

                            map.setPixel(j, i, pixel);
                        }
                    saveImageRGBA("test.png", map);
                }
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