#include <set>
#include "imagesaver.h"
#include "logic.h"

using namespace std;

enum COLOUR
{
    NONE = 0,
    WHITE = 1,
    RED = 2,
    GREEN = 4,
    BLUE = 8
};

bool pixelIsWhite(const pixelRGBA &pixel)
{
    if (pixel.r > 127 && pixel.g > 127 && pixel.b > 127)
        return true;
    else
        return false;
}

const int moves[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

// void regionSearch(int rootX, int rootY, const imageRGBA &image, bool *checker)
// {
//     unsigned int position = 0;
//     int width = image.getWidth();
//     int height = image.getHeight();
//     while (position < root.coordinates.size())
//     {
//         for (int i = 0; i < 4; i++)
//         {
//             int tempX = root.coordinates[position].x + moves[i][0];
//             int tempY = root.coordinates[position].y + moves[i][1];
//             if (tempX > -1 && tempX < width && tempY > -1 && tempY < height)
//                 if (!checker[tempY * width + tempX])
//                 {
//                     checker[tempY * width + tempX] = true;
//                     if (pixelIsWhite(image.getPixel(tempX, tempY)))
//                     {
//                         regionChecher[tempY * width + tempX] = counter;
//                         root.coordinates.push_back(coordinate{tempX, tempY});
//                     }
//                 }
//         }
//         position++;
//     }
// }

void recursiveSearch(int rootX, int rootY, int number, const imageRGBA &image, int **regions, bool **checker)
{
    for (int i = 0; i < 4; i++)
    {
        int positionX = rootX + moves[i][0];
        int positionY = rootY + moves[i][1];
        if (positionX > -1 && positionX < image.getWidth() && positionY > -1 && positionY < image.getHeight())
        {
            if (!checker[positionY][positionX])
            {
                if (pixelIsWhite(image.getPixel(positionX, positionY)))
                {
                    regions[positionY][positionX] = number;
                    checker[positionY][positionX] = true;
                    recursiveSearch(positionX, positionY, number, image, regions, checker);
                }
            }
        }
    }
}

//TODO: deallocate checher and company;
int readImage(const imageRGBA &image, int **regions, bool **checker)
{
    int width = image.getWidth();
    int height = image.getHeight();
    int regionsCounter = 1;

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if (!checker[i][j])
            {
                if (pixelIsWhite(image.getPixel(j, i)))
                {
                    regions[i][j] = regionsCounter;
                    checker[i][j] = true;
                    recursiveSearch(j, i, regionsCounter, image, regions, checker);
                    regionsCounter++;
                }
            }

    //remove borders
    regions[0][0] = 1;
    checker[0][0] = true;
    for (int i = 0; i < height; i++)
    {
        if (!checker[i][0])
        {
            regions[i][0] = regions[i - 1][0];
        }
        for (int j = 1; j < width; j++)
        {
            if (!checker[i][j])
            {
                regions[i][j] = regions[i][j - 1];
            }
        }
    }

    return regionsCounter;
}

void solveMap(imageRGBA &image)
{
    int width = image.getWidth();
    int height = image.getHeight();

    int **regions = new int *[height];
    for (int i = 0; i < height; i++)
        regions[i] = new int[width];
    bool **checker = new bool *[height];
    for (int i = 0; i < height; i++)
        checker[i] = new bool[width];

    int regionsNumber = readImage(image, regions, checker);
    set<int> *links = new set<int>[regionsNumber + 1];

    //TODO: combine these two searches
    for (int i = 0; i < height; i++)
    {
        int lastRegion = 0;
        for (int j = 0; j < width; j++)
        {
            int newRegion = regions[i][j];
            if (lastRegion != newRegion)
            {
                if (links[newRegion].find(lastRegion) == links[newRegion].end())
                {
                    links[newRegion].insert(lastRegion);
                    links[lastRegion].insert(newRegion);
                }
                lastRegion = newRegion;
            }
        }
    }
    for (int j = 0; j < width; j++)
    {
        int lastRegion = 0;
        for (int i = 0; i < height; i++)
        {
            int newRegion = regions[i][j];
            if (lastRegion != newRegion)
            {
                if (links[newRegion].find(lastRegion) == links[newRegion].end())
                {
                    links[newRegion].insert(lastRegion);
                    links[lastRegion].insert(newRegion);
                }
                lastRegion = newRegion;
            }
        }
    }

    // 15 = RED + GREEN + BLUE + WHITE
    int *possibleColours = new int[regionsNumber + 1];
    for (int i = 1; i < regionsNumber + 1; i++)
        possibleColours[i] = 15;
    COLOUR *definitiveColour = new COLOUR[regionsNumber + 1];
    for (int i = 1; i < regionsNumber + 1; i++)
        definitiveColour[i] = NONE;

    for (int i = 1; i < regionsNumber + 1; i++)
    {
        COLOUR selectedColour;
        if (possibleColours[i] & WHITE)
            selectedColour = WHITE;
        else if (possibleColours[i] & RED)
            selectedColour = RED;
        else if (possibleColours[i] & GREEN)
            selectedColour = GREEN;
        else if (possibleColours[i] & BLUE)
            selectedColour = BLUE;
        for (int link : links[i])
        {
            if (possibleColours[link] & selectedColour)
                possibleColours[link] -= selectedColour;
        }
        definitiveColour[i] = selectedColour;
    }

    //colouring
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            if (checker[i][j])
            {
                pixelRGBA colour;
                switch (definitiveColour[regions[i][j]])
                {
                case RED:
                    colour.r = 255;
                    break;
                case GREEN:
                    colour.g = 255;
                    break;
                case BLUE:
                    colour.b = 255;
                    break;
                case WHITE:
                    colour.r = 255;
                    colour.g = 255;
                    colour.b = 255;
                    break;
                default:
                    break;
                }
                image.setPixel(j, i, colour);
            }
        }

    //deleting stuff
    for (int i = 0; i < height; i++)
        delete[] regions[i];
    delete[] regions;
    for (int i = 0; i < height; i++)
        delete[] checker[i];
    delete[] checker;
    delete[] possibleColours;
    delete[] definitiveColour;
    delete[] links;
}