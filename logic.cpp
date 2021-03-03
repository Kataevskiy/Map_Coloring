#include <vector>
#include "imagesaver.h"
#include "logic.h"

using namespace std;

enum class COLOUR
{
    WHITE,
    RED,
    GREED,
    BLUE,
    NONE
};

struct coordinate
{
    int x;
    int y;
};

struct region
{
    vector<coordinate> coordinates;
};

vector<region> regions;

const int moves[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

bool pixelIsWhite(pixelRGBA const &pixel)
{
    if (pixel.r > 127 && pixel.g > 127 && pixel.b > 127)
        return true;
    else
        return false;
}

void recursiveSearch(region &root, imageRGBA &image, bool *checker)
{
    unsigned int position = 0;
    while (position < root.coordinates.size())
    {
        for (int i = 0; i < 4; i++)
        {
            int tempX = root.coordinates[position].x + moves[i][0];
            int tempY = root.coordinates[position].y + moves[i][1];
            if (tempX > -1 && tempX < image.getWidth() && tempY > -1 && tempY < image.getHeight())
                if (!checker[tempY * image.getWidth() + tempX])
                {
                    checker[tempY * image.getWidth() + tempX] = true;
                    if (pixelIsWhite(image.getPixel(tempX, tempY)))
                        root.coordinates.push_back(coordinate{tempX, tempY});
                }
        }
        position++;
    }
}

void readImage(imageRGBA &image)
{
    int width = image.getWidth();
    int height = image.getHeight();
    bool *checker = new bool[width * height];
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if (!checker[i * width + j])
            {
                coordinate newCoordinate{j, i};
                region newRegion;
                newRegion.coordinates.push_back(newCoordinate);
                recursiveSearch(newRegion, image, checker);
                regions.push_back(newRegion);
            }
    delete[] checker;
}

void paintMap(imageRGBA &image)
{
    int R = 0, G = 0, B = 0;
    for (const region &currentRegion : regions)
    {
        R = (R + 77) % 256;
        G = (G + 55) % 256;
        B = (B + 33) % 256;
        for (const coordinate &currentCoordinate : currentRegion.coordinates)
        {
            pixelRGBA colour{R, G, B, 255};
            image.setPixel(currentCoordinate.x, currentCoordinate.y, colour);
        }
    }
}
