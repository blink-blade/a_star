#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct vec2d {
    double x, y;
};

struct corner {
    double x;
    double y;
    struct vec2d gradientVec;
};

struct tile {
    int tileX;
    int tileY;
    int type;
    int right;
    int top;
};

int width = 10;
int height = 10;
unsigned int seed = 100;
int **mapArray;
float **noiseMap;
struct tile **tiles;
int walkable_tile_types[] = {2, 3, 4};

struct vec2d possibleGradientVectors[8];

struct vec2d randomGradient(int ix, int iy) {

    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; 
    unsigned a = ix, b = iy;
    a *= 3284157443 * seed;
 
    b ^= a << s | a >> w - s;
    b *= 1911520717 * seed;
 
    a ^= b << s | b >> w - s;
    a *= 2048419325 * seed;
    struct vec2d v;
    v.x = sin(a);
    v.y = cos(a);
    return v;
}


int valueInArray(float val, int *arr, size_t n) {
    for(size_t i = 0; i < n; i++) {
        if(arr[i] == val)
            return 1;
    }
    return 0;
}


double nearestTenth(float val) {
    int len = log10(val);
    float div = pow(10, len);
    return ceil(val / div) * div;
}

// Computes the dot product of the distance and gradient vectors.
double dotGridGradient(int ix, int iy, double x, double y) {
    
    // Get gradient from integer coordinates
    struct vec2d gradient = randomGradient(ix, iy);
 
    // Compute the distance vector
    double dx = x - (double)ix;
    double dy = y - (double)iy;
 
    // Compute the dot-product
    return (dx * gradient.x + dy * gradient.y);
}
 
double interpolate(double a0, double a1, double w) {
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}
 
 
// Sample Perlin noise at coordinates x, y
double noise(double x, double y) {

    // Get the corner positions, x0 is left, x1 is left
    int x0 = (int)x; 
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Compute Interpolation weights
    double sx = x - (double)x0;
    double sy = y - (double)y0;

    struct corner tlCorner;
    struct corner blCorner;
    struct corner trCorner;
    struct corner brCorner;
    tlCorner.x = x0, tlCorner.y = y0;
    blCorner.x = x0, blCorner.y = y1;
    trCorner.x = x1, trCorner.y = y0;
    brCorner.x = x1, brCorner.y = y1;
    tlCorner.gradientVec = randomGradient(tlCorner.x, tlCorner.y);
    blCorner.gradientVec = randomGradient(blCorner.x, blCorner.y);
    trCorner.gradientVec = randomGradient(trCorner.x, trCorner.y);
    brCorner.gradientVec = randomGradient(brCorner.x, brCorner.y);

    // Get distance vectors(A vector from the corner which points to the tile) for each corner of the octant.
    struct vec2d tlDistanceVector;
    struct vec2d blDistanceVector;
    struct vec2d trDistanceVector;
    struct vec2d brDistanceVector;
    tlDistanceVector.x = x - tlCorner.x, tlDistanceVector.y = y - tlCorner.y; 
    blDistanceVector.x = x - blCorner.x, blDistanceVector.y = y - blCorner.y; 
    trDistanceVector.x = x - trCorner.x, trDistanceVector.y = y - trCorner.y; 
    brDistanceVector.x = x - brCorner.x, brDistanceVector.y = y - brCorner.y; 

    // Get dot product from the distance vectors and the gradient vectors.
    double tlDotProduct = (tlDistanceVector.x * tlCorner.gradientVec.x) + (tlDistanceVector.y * tlCorner.gradientVec.y);
    double blDotProduct = (blDistanceVector.x * blCorner.gradientVec.x) + (blDistanceVector.y * blCorner.gradientVec.y);
    double trDotProduct = (trDistanceVector.x * trCorner.gradientVec.x) + (trDistanceVector.y * trCorner.gradientVec.y);
    double brDotProduct = (brDistanceVector.x * brCorner.gradientVec.x) + (brDistanceVector.y * brCorner.gradientVec.y);

    // Interpolation.
    double tlTrInterpolation = interpolate(tlDotProduct, trDotProduct, sx);
    double blBrInterpolation = interpolate(blDotProduct, brDotProduct, sx);

    
    return interpolate(tlTrInterpolation, blBrInterpolation, sy);
}
 
void generateMap(int newWidth, int newHeight, int layerAmount, double freq, double octaves, unsigned int newSeed) {
    int x, y, i;
    // printf("%d %d %d %f %d \n", newWidth, newHeight, newOctaves, freq, newSeed);
    // Make a pointer for each global variable, so we can modify them.
    int * pWidth = &width;
    int * pHeight = &height;
    unsigned int * pSeed = &seed;

    // Give the pointers of the global variables their value. This modifies the global variables!
    *pWidth = newWidth;
    *pHeight = newHeight;
    *pSeed = newSeed;

    struct vec2d gradientVecOne;
    struct vec2d gradientVecTwo;
    struct vec2d gradientVecThree;
    struct vec2d gradientVecFour;
    struct vec2d gradientVecFive;
    struct vec2d gradientVecSix;
    struct vec2d gradientVecSeven;
    struct vec2d gradientVecEight;
    gradientVecOne.x = -1, gradientVecOne.y = -1;
    gradientVecTwo.x = -1, gradientVecTwo.y = 1;
    gradientVecThree.x = 1, gradientVecThree.y = -1;
    gradientVecFour.x = 1, gradientVecFour.y = 1;
    gradientVecFive.x = 0, gradientVecFive.y = -sqrt(2);
    gradientVecSix.x = 0, gradientVecSix.y = sqrt(2);
    gradientVecSeven.x = sqrt(2), gradientVecSeven.y = 0;
    gradientVecEight.x = -sqrt(2), gradientVecEight.y = 0;
    possibleGradientVectors[0] = gradientVecOne, possibleGradientVectors[1] = gradientVecTwo, possibleGradientVectors[2] = gradientVecThree, possibleGradientVectors[3] = gradientVecFour;
    possibleGradientVectors[4] = gradientVecFive, possibleGradientVectors[5] = gradientVecSix, possibleGradientVectors[6] = gradientVecSeven, possibleGradientVectors[7] = gradientVecEight;

    // Allocate memory for the map array.
    mapArray = malloc(sizeof(int*)*height);
    // Make rows in the map array.
    for(i=0; i<height + 1; i++) {
        mapArray[i] = malloc(sizeof(int) * width);
    }
    // Allocate memory for the tiles array.
    tiles = malloc(sizeof(struct tile*)*height);
    // Make rows in the tiles array.
    for(i=0; i<height + 1; i++) {
        tiles[i] = malloc(sizeof(struct tile) * width);
    }
    // Allocate memory for the array.
    noiseMap = malloc(sizeof(float*)*height);
    // Make rows in the array.
    for(i=0; i<height + 1; i++) {
        noiseMap[i] = malloc(sizeof(float) * width);
    }
    double octaveX = (octaves * ((double)width / 100));
    double octaveY = (octaves * ((double)height / 100));
    double val = 0;
    double divide_amount = ((double)(width / 6) + (double)(height / 6));
    double distance_to_center;
    float div;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            float amp = 1;
            val = 0;
            freq = 1.5;
            for (int i = 0; i < layerAmount; i++)
            {

                val += noise((double)x * freq / octaveX, (double)y * freq / octaveY) * amp;

                freq *= 2;
                amp /= 2;
            }

            // Using the pythagoras theorem, calculate the distance from the center of the map. Then change the value depending on that distance, this makes it an island shape.
            distance_to_center = sqrt(pow((x - width / 2), 2) + pow((y - height / 2), 2));
            val = fabs(val) - (distance_to_center / divide_amount);
            
            // Clip the values.
            if (val > 1)
                val = 1;
            else if (val < -1)
                val = -1;
            noiseMap[y][x] = val;
            // Set the the tile type depending on the value.
            if (val < -0.55) {
                val = 1;
            } else if (val < -0.3) {
                val = 2;
            } else if (val < 2) {
                val = 3;
            }
            
            // Init the tile for this position.
            struct tile currentTile;
            currentTile.tileX = x;
            currentTile.tileY = y;
            currentTile.right = x + 1;
            currentTile.top = y + 1;
            currentTile.type = val;
            tiles[y][x] = currentTile;

            mapArray[y][x] = val;
        }
    }


}
// This is for getting the noise values with python.
double getTileType(int x, int y) {
    return mapArray[y][x];
}

// This is for getting the noise values with python.
double getNoiseValue(int x, int y) {
    return noiseMap[y][x];
}

// This is for getting the noise values with python.
double getWaterOpacity(int x, int y) {
    return fabs(noiseMap[y][x] / 3);
}

void setTile(int x, int y, int type) {
    mapArray[y][x] = type;
    tiles[y][x].type = type;
}
// Things todo next:
// Make a function to get the dot product between distance vectors and gradient vectors.

int pointCollidesWithRect(double pointX, double pointY, int left, int right, int bottom, int top) {
    if (pointX > right) {
        return 0;
    }
    if (pointX < left) {
        return 0;
    }
    if (pointY > top) {
        return 0;
    }
    if (pointY < bottom) {
        return 0;
    }
    return 1;
}


double handleCollisionsX(double x, double y, double width, double height, double halfWidth, double halfHeight) {
    struct tile leftTile = tiles[(int)(y + halfHeight)][(int)x];
    if (!valueInArray(leftTile.type, walkable_tile_types, 3)) {
        if (pointCollidesWithRect(x, y + halfHeight, leftTile.tileX, leftTile.right, leftTile.tileY, leftTile.top)) {
            x = leftTile.right;
        }
    }

    struct tile rightTile = tiles[(int)(y + halfHeight)][(int)(x + width)];
    if (!valueInArray(rightTile.type, walkable_tile_types, 3)) {
        if (pointCollidesWithRect(x + width , y + halfHeight, rightTile.tileX, rightTile.right, rightTile.tileY, rightTile.top)) {
            x = rightTile.tileX - width;
        }
    }
    return x;
}

double handleCollisionsY(double x, double y, double width, double height, double halfWidth, double halfHeight) {
    struct tile bottomTile = tiles[(int)y][(int)(x + halfWidth)];
    if (!valueInArray(bottomTile.type, walkable_tile_types, 3)) {
        if (pointCollidesWithRect(x + halfWidth, y, bottomTile.tileX, bottomTile.right, bottomTile.tileY, bottomTile.top)) {
            y = bottomTile.top;
        }
    }

    struct tile topTile = tiles[(int)(y + height)][(int)(x + halfWidth)];
    if (!valueInArray(topTile.type, walkable_tile_types, 3)) {
        if (pointCollidesWithRect(x + halfWidth, y + height, topTile.tileX, topTile.right, topTile.tileY, topTile.top)) {
            y = topTile.tileY - height;
        }
    }
    return y;
}