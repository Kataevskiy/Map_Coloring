#include <SDL.h>
#include <iostream>
#include "imagesaver.h"
#include "logic.h"

using namespace std;

const int width = 1600, height = 900;

void renderImage(imageRGBA &image, SDL_Renderer *renderer)
{
    int width = image.getWidth();
    int height = image.getHeight();
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            pixelRGBA colour = image.getPixel(j, i);
            SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
            SDL_RenderDrawPoint(renderer, j, i);
        }
}

imageRGBA saveScreen(SDL_Renderer *renderer)
{
    SDL_Surface *tempSurface = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, tempSurface->pixels, tempSurface->pitch);
    imageRGBA tempImage(width, height);
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            pixelRGBA pixel;
            pixel.r = (Uint8)(((Uint32 *)(tempSurface->pixels))[i * width + j] >> 16);
            pixel.g = (Uint8)(((Uint32 *)(tempSurface->pixels))[i * width + j] >> 8);
            pixel.b = (Uint8)(((Uint32 *)(tempSurface->pixels))[i * width + j] >> 0);
            pixel.a = (Uint8)(((Uint32 *)(tempSurface->pixels))[i * width + j] >> 24);

            tempImage.setPixel(j, i, pixel);
        }
    SDL_FreeSurface(tempSurface);
    return tempImage;
}

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
                if (e.key.keysym.sym == SDLK_s)
                {
                    imageRGBA image = saveScreen(mainRenderer);
                    saveImageRGBA("test.png", image);
                }
                else if (e.key.keysym.sym == SDLK_RETURN)
                {
                    imageRGBA image = saveScreen(mainRenderer);
                    readImage(image);
                    paintMap(image);
                    renderImage(image, mainRenderer);
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