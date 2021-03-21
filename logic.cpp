#include <vector>
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

struct coordinate
{
    int x;
    int y;
};

struct region
{
    vector<coordinate> coordinates;
    vector<region *> links;
    int possibleColour = 15;
    COLOUR definitiveColour = NONE;
};

bool pixelIsWhite(const pixelRGBA &pixel)
{
    if (pixel.r > 127 && pixel.g > 127 && pixel.b > 127)
        return true;
    else
        return false;
}

const int moves[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

void regionSearch(region &root, const imageRGBA &image, bool *checker, int *regionChecher, int counter)
{
    unsigned int position = 0;
    int width = image.getWidth();
    int height = image.getHeight();
    while (position < root.coordinates.size())
    {
        for (int i = 0; i < 4; i++)
        {
            int tempX = root.coordinates[position].x + moves[i][0];
            int tempY = root.coordinates[position].y + moves[i][1];
            if (tempX > -1 && tempX < width && tempY > -1 && tempY < height)
                if (!checker[tempY * width + tempX])
                {
                    checker[tempY * width + tempX] = true;
                    if (pixelIsWhite(image.getPixel(tempX, tempY)))
                    {
                        regionChecher[tempY * width + tempX] = counter;
                        root.coordinates.push_back(coordinate{tempX, tempY});
                    }
                }
        }
        position++;
    }
}

vector<region> regions;
int *regionChecker;

void readImage(const imageRGBA &image)
{
    int width = image.getWidth();
    int height = image.getHeight();
    int regionCounter = 1;
    bool *checker = new bool[width * height];
    regionChecker = new int[width * height];
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if (!checker[i * width + j])
            {
                checker[i * width + j] = true;
                if (pixelIsWhite(image.getPixel(j, i)))
                {
                    regionChecker[i * width + j] = regionCounter;
                    coordinate newCoordinate{j, i};
                    region newRegion;
                    newRegion.coordinates.push_back(newCoordinate);
                    regionSearch(newRegion, image, checker, regionChecker, regionCounter);
                    regions.push_back(newRegion);
                    regionCounter++;
                }
            }
    delete[] checker;
}

void makeLink(vector<region> &regions, int first, int second, bool *checker)
{
    if (!checker[first * regions.size() + second])
    {
        checker[first * regions.size() + second] = true;
        checker[second * regions.size() + first] = true;
        regions[first].links.push_back(&regions[second]);
        regions[second].links.push_back(&regions[first]);
    }
}

void makeLinks(const imageRGBA &image, vector<region> &regions, int *regionChecker)
{
    int width = image.getWidth();
    int height = image.getHeight();
    bool *checker = new bool[regions.size() * regions.size()];
    for (int i = 0; i < height; i++)
    {
        int lastLink = 0;
        for (int j = 0; j < width; j++)
        {
            int newLink = regionChecker[i * width + j];
            if (newLink != 0 && newLink != lastLink)
            {
                // 0 stands for black wall so regions begin with 1
                if (lastLink != 0)
                    makeLink(regions, lastLink - 1, newLink - 1, checker);
                lastLink = newLink;
            }
        }
    }
    // for (int j = 0; j < width; j++)
    // {
    //     int lastLink = 0;
    //     for (int i = 0; i < height; i++)
    //     {
    //         int newLink = regionChecker[i * width + j];
    //         if (newLink != 0 && newLink != lastLink)
    //         {
    //             if (lastLink != 0)
    //                 makeLink(regions, lastLink - 1, newLink - 1, checker);
    //             lastLink = newLink;
    //         }
    //     }
    // }
    delete[] regionChecker;
    delete[] checker;
}

void solveMap(imageRGBA &image)
{
    readImage(image);
    makeLinks(image, regions, regionChecker);
    for (region &currentRegion : regions)
    {
        COLOUR usedColour;
        if (currentRegion.possibleColour & BLUE)
            usedColour = BLUE;
        else if (currentRegion.possibleColour & GREEN)
            usedColour = GREEN;
        else if (currentRegion.possibleColour & RED)
            usedColour = RED;
        else
            usedColour = WHITE;
        currentRegion.definitiveColour = usedColour;
        for (region *linkRegion : currentRegion.links)
            if (linkRegion->possibleColour > usedColour)
                linkRegion->possibleColour -= usedColour;
    }
    for (region &currentRegion : regions)
    {
        int R = 0, G = 0, B = 0;
        switch (currentRegion.definitiveColour)
        {
        case RED:
            R = 255;
            break;
        case GREEN:
            G = 255;
            break;
        case BLUE:
            B = 255;
            break;
        case WHITE:
            R = 255;
            G = 255;
            B = 255;
            break;
        default:
            break;
        }
        pixelRGBA colour{R, G, B, 255};
        for (coordinate currentCoordinate : currentRegion.coordinates)
        {
            image.setPixel(currentCoordinate.x, currentCoordinate.y, colour);
        }
    }
}