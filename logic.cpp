#include <set>
#include <queue>
#include <algorithm>
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

const int fourMoves[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
const int eightMoves[8][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

void recursiveSearch(int rootX, int rootY, int number, const imageRGBA &image, int **regions, bool **checker)
{
    queue<pair<int, int>> cellsQueue;
    cellsQueue.emplace(rootY, rootX);
    while (cellsQueue.size() > 0)
    {
        pair<int, int> coordinate = cellsQueue.front();
        for (int i = 0; i < 4; i++)
        {
            int positionX = coordinate.second + fourMoves[i][0];
            int positionY = coordinate.first + fourMoves[i][1];
            if (positionX > -1 && positionX < image.getWidth() && positionY > -1 && positionY < image.getHeight())
            {
                if (!checker[positionY][positionX])
                {
                    if (pixelIsWhite(image.getPixel(positionX, positionY)))
                    {
                        regions[positionY][positionX] = number;
                        checker[positionY][positionX] = true;
                        cellsQueue.emplace(positionY, positionX);
                    }
                }
            }
        }
        cellsQueue.pop();
    }
}

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

    //TODO: maybe replace with better algorithm?
    //replace border element with any most common adjacent region element.
    for (int i = 1; i < height - 1; i++)
        for (int j = 1; j < width - 1; j++)
            if (!checker[i][j])
            {
                int cells[8];
                for (int k = 0; k < 8; k++)
                    cells[k] = regions[i + eightMoves[k][0]][j + eightMoves[k][1]];
                sort(begin(cells), end(cells));
                int maxCounter = -1;
                int maxValue = 0;
                int lastValue = -1;
                int counter = 0;
                for (int k = 0; k < 8; k++)
                {
                    int newValue = cells[k];
                    if (newValue != 0)
                    {
                        if (newValue == lastValue)
                        {
                            counter++;
                            if ((counter > maxCounter) || (counter == maxCounter && newValue > maxValue))
                            {
                                maxCounter = counter;
                                maxValue = newValue;
                            }
                        }
                        else
                        {
                            lastValue = newValue;
                            counter = 0;
                        }
                    }
                }
                regions[i][j] = maxValue;
            }

    return regionsCounter;
}

//TODO: encapsulate it better
void recursiveColourPicking(int regionsNumber, int **regions, set<int> *links, int *possibleColours, COLOUR *definitiveColour)
{
    queue<int> regionsQueue;
    regionsQueue.push(1);
    while (regionsQueue.size() > 0)
    {
        int currentRegion = regionsQueue.front();
        COLOUR selectedColour;
        if (possibleColours[currentRegion] & WHITE)
            selectedColour = WHITE;
        else if (possibleColours[currentRegion] & RED)
            selectedColour = RED;
        else if (possibleColours[currentRegion] & GREEN)
            selectedColour = GREEN;
        else if (possibleColours[currentRegion] & BLUE)
            selectedColour = BLUE;
        definitiveColour[currentRegion] = selectedColour;
        for (int link : links[currentRegion])
        {
            if (possibleColours[link] & selectedColour)
                possibleColours[link] -= selectedColour;
            links[link].erase(currentRegion);
            regionsQueue.push(link);
        }
        regionsQueue.pop();
    }
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
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            regions[i][j] = 0;
            checker[i][j] = false;
        }

    int regionsNumber = readImage(image, regions, checker);
    set<int> *links = new set<int>[regionsNumber + 1];

    //TODO: combine these two searches
    for (int i = 0; i < height; i++)
    {
        int lastRegion = 1;
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
        int lastRegion = 1;
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

    //region 0 is for borders
    for (int link : links[0])
    {
        links[link].erase(0);
        links[0].erase(link);
    }
    recursiveColourPicking(0, regions, links, possibleColours, definitiveColour);

    //colouring
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            // if (checker[i][j])
            // {
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
            // }
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