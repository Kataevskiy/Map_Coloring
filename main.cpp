#include <SDL.h>
#include <string>
#include <iostream>
#include "imagesaver.h"
#include "logic.h"

using namespace std;

const int windowWidth = 1600, windowHeight = 900;

imageRGBA saveScreen(SDL_Renderer *renderer)
{
    SDL_Surface *tempSurface = SDL_CreateRGBSurface(0, windowWidth, windowHeight, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, tempSurface->pixels, tempSurface->pitch);
    imageRGBA tempImage(windowWidth, windowHeight);
    for (int i = 0; i < windowHeight; i++)
        for (int j = 0; j < windowWidth; j++)
        {
            colourRGBA pixel;
            pixel.r = (Uint8)(((Uint32 *)(tempSurface->pixels))[i * windowWidth + j] >> 16);
            pixel.g = (Uint8)(((Uint32 *)(tempSurface->pixels))[i * windowWidth + j] >> 8);
            pixel.b = (Uint8)(((Uint32 *)(tempSurface->pixels))[i * windowWidth + j] >> 0);
            pixel.a = (Uint8)(((Uint32 *)(tempSurface->pixels))[i * windowWidth + j] >> 24);

            tempImage.setPixel(j, i, pixel);
        }
    SDL_FreeSurface(tempSurface);
    return tempImage;
}

//not particularly efficient.
void renderImage(imageRGBA &image, SDL_Renderer *renderer)
{
    int width = image.getWidth();
    int height = image.getHeight();
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            colourRGBA colour = image.getPixel(j, i);
            SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
            SDL_RenderDrawPoint(renderer, j, i);
        }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *mainWindow = SDL_CreateWindow("Map_Coloring", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer *mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(mainRenderer, 255, 255, 255, 255);
    SDL_RenderClear(mainRenderer);
    SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 255);

    string path;
    cout << "path of starting image or \"none\"" << '\n';
    imageRGBA startingImage;
    while (startingImage.getWidth() == 0)
    {
        cin >> path;
        if (path == "none")
            break;
        startingImage = loadImageRGBA(path.c_str());
    }
    if (path != "none")
        renderImage(startingImage, mainRenderer);

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
                    solveMap(image);
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
                SDL_RenderDrawLine(mainRenderer, lastX, lastY + 1, x, y + 1);
                SDL_RenderDrawLine(mainRenderer, lastX + 1, lastY, x + 1, y);
                SDL_RenderDrawLine(mainRenderer, lastX + 1, lastY + 1, x + 1, y + 1);
                lastX = x;
                lastY = y;
            }
        }

        SDL_RenderPresent(mainRenderer);
    }

    return 1;
}