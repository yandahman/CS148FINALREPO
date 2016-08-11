#ifndef TERRAIN_GRID_H
#define TERRAIN_GRID_H

// GLM Mathematics
#define GLM_FORCE_RADIANS // force everything in radian
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

//Header File for Terrain Generator
using namespace std;
//has to be equal to (2^x) + 1
const int X_SCALE = 1025;
const int Z_SCALE = 1025;
//const int recursiveSteps = 11;
const float initHeightMax = 0;
const float MAP_SCALE = .03;
const float MAX_SLOPE = .01;//.1
const float ROUGH_SCALE = 1.0;


//This is supposed to be the max possible height but its kinda guesstimated.
const float maxPossibleHeight =  (initHeightMax + (float)sqrt((float)pow(((MAP_SCALE*X_SCALE/2) + 1),2) + (float)pow(((MAP_SCALE * X_SCALE/2) + 1),2)))/1.5f;

struct midPoint{
    int x;
    int z;
};

float initialize(float*** terrain);

//void SquareStep(float*** terrain,int xMin, int zMin, int xMax, int zMax, int xMid, int zMid,float &testMaxHeight, float &testMinHeight);

void squareStep(float*** terrain, int x, int z, int dWidth, float offset);

//midPoint DiamondSquareStep(float*** terrain,int xMin, int zMin, int xMax, int zMax, float &testMaxHeight, float &testMinHeight);

void diamondStep(float*** terrain, int x, int z, int sWidth,float offset);

//void DiamondSquare(float*** terrain,int xMin, int zMin, int xMax, int zMax, int recursiveSteps, float &testMaxHeight, float &testMinHeight);

void diamondSquare(float*** terrain, int width, float rangeFactor);

int GetNumberOfNeighbors(int x, int z);

int ** neighborFinder(int x, int z, int numberOfNeighbors);

void laplacianSmoothing(float*** terrain, int recursion);

glm::vec3* neighborVectors(float ***terrain, int **n, int numNeighbors);

void GenerateNormals(float ***normals, float*** terrain);

#endif

