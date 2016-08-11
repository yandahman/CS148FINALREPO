#include "TerrainGrid.h"
#include <iostream>
#include <stdlib.h>
#include <cmath>

// GLM Mathematics
#define GLM_FORCE_RADIANS // force everything in radian
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

float initialize(float*** terrain) {
    cout << maxPossibleHeight << endl;
    for (int x = 0; x < X_SCALE; x++){
       for (int z = 0; z < Z_SCALE; z++){
           terrain[x][z][0] = (float)(x)*MAP_SCALE; //x info for point
           terrain[x][z][1] = (float)0.0;         //y info for point
           terrain[x][z][2] = (float)(z)*MAP_SCALE; //z info for point
       }
    } 
    srand(time(NULL));
    //cout << initHeightMax << endl;
    terrain[0][0][1] = (((float)rand())/(float)RAND_MAX)*2*initHeightMax-initHeightMax;
    //cout << terrain[0][0][1] << endl;
    terrain[0][Z_SCALE-1][1] = (((float)rand())/(float)RAND_MAX)*2*initHeightMax-initHeightMax;
    //cout << terrain[0][Z_SCALE-1][1]<<endl;
    terrain[X_SCALE-1][0][1] = (((float)rand())/(float)RAND_MAX)*2*initHeightMax-initHeightMax;
    //cout << terrain[X_SCALE-1][1][1]<<endl;
    terrain[X_SCALE-1][Z_SCALE-1][1] = (((float)rand())/(float)RAND_MAX)*2*initHeightMax-initHeightMax;
    //cout << terrain[X_SCALE-1][Z_SCALE-1][1]<<endl;
    return maxPossibleHeight;
}

void squareStep(float*** terrain, int x, int z, int dWidth, float offset) {
    float avg = 0;
    //LEFT
    if(x == 0) {
        avg = terrain[x][z-dWidth][1] + terrain[x+dWidth][z][1] + terrain[x][z+dWidth][1];
        avg/=3.0;

    //RIGHT
    }else if(x == X_SCALE-1) {
        avg = terrain[x][z-dWidth][1] + terrain[x-dWidth][z][1] + terrain[x][z+dWidth][1];
        avg/=3.0;

    //TOP
    }else if(z == 0){
        avg = terrain[x+dWidth][z][1] + terrain[x-dWidth][z][1] + terrain[x][z+dWidth][1];
        avg/=3.0;

    //BOTTOM
    }else if(z ==Z_SCALE-1){
        avg = terrain[x][z-dWidth][1] + terrain[x+dWidth][z][1] + terrain[x-dWidth][z][1];
        avg/=3.0;
    
    //Regular squareStep
    }else{
        avg = terrain[x][z-dWidth][1] + terrain[x+dWidth][z][1] + terrain[x-dWidth][z][1] + terrain[x][z+dWidth][1];
        avg/=4.0;
    }

    //update point with avg+offset
    terrain[x][z][1] = avg + offset;
}
/*
void SquareStep(float*** terrain,int xMin, int zMin, int xMax, int zMax, int xMid, int zMid, float &testMaxHeight, float &testMinHeight){
    int x,z;
    int randomHeight;
    float heightDisplacement;
    //left
    x = xMin;
    z = (zMin + zMax)/2;
    if (xMin - xMid < 0){
        heightDisplacement = terrain[xMin][zMin][1] + terrain[xMin][zMax][1] + terrain[xMid][zMid][1];
        heightDisplacement = heightDisplacement/3;
    }else {
        heightDisplacement = terrain[xMin][zMin][1] + terrain[xMin][zMax][1] + terrain[xMid][zMid][1] + terrain[xMin - xMid][zMid][1];
        heightDisplacement = heightDisplacement/4;
    }
    randomHeight = rand() % (2*(zMax - zMin));
    randomHeight = randomHeight - (zMax - zMin);
    randomHeight = randomHeight/2;
    terrain[x][z][1] = (randomHeight * MAX_SLOPE) + heightDisplacement;

    if (terrain[x][z][1] < testMinHeight){
        testMinHeight = terrain[x][z][1];
    }
    if (terrain[x][z][1] > testMaxHeight){
        testMaxHeight = terrain[x][z][1];
    }

    //right
    x = xMax;
    z = (zMin + zMax)/2;
    if (xMax + xMid > X_SCALE-1){
        heightDisplacement = terrain[xMax][zMin][1] + terrain[xMax][zMax][1] + terrain[xMid][zMid][1];
        heightDisplacement = heightDisplacement/3;
    }else{
        heightDisplacement = terrain[xMax][zMin][1] + terrain[xMax][zMax][1] + terrain[xMid][zMid][1] + terrain[xMax + xMid][zMid][1];
        heightDisplacement = heightDisplacement/4;        
    }
    randomHeight = rand() % (2*(zMax - zMin));
    randomHeight = randomHeight - (zMax - zMin);
    randomHeight = randomHeight/2;
    terrain[x][z][1] = (randomHeight * MAX_SLOPE) + heightDisplacement;

    if (terrain[x][z][1] < testMinHeight){
        testMinHeight = terrain[x][z][1];
    }
    if (terrain[x][z][1] > testMaxHeight){
        testMaxHeight = terrain[x][z][1];
    }

    //top
    x = (xMin + xMax)/2;
    z = zMin;
    if (zMin - zMid < 0){
        heightDisplacement = terrain[xMin][zMin][1] + terrain[xMax][zMin][1] + terrain[xMid][zMid][1];
        heightDisplacement = heightDisplacement/3;
    }else{
        heightDisplacement = terrain[xMin][zMin][1] + terrain[xMax][zMin][1] + terrain[xMid][zMid][1] + terrain[xMid][zMin - zMid][1];
        heightDisplacement = heightDisplacement/4;        
    }
    randomHeight = rand() % (2*(xMax - xMin));
    randomHeight = randomHeight - (xMax - xMin);
    randomHeight = randomHeight/2;  
    terrain[x][z][1] = (randomHeight * MAX_SLOPE) + heightDisplacement;

    if (terrain[x][z][1] < testMinHeight){
        testMinHeight = terrain[x][z][1];
    }
    if (terrain[x][z][1] > testMaxHeight){
        testMaxHeight = terrain[x][z][1];
    }

    //bottom
    x = (xMin + xMax)/2;
    z = zMax;
    if (zMax + zMid > Z_SCALE-1){
        heightDisplacement = terrain[xMin][zMax][1] + terrain[xMax][zMax][1] + terrain[xMid][zMid][1];
        heightDisplacement = heightDisplacement/3;
    }else{
        heightDisplacement = terrain[xMin][zMax][1] + terrain[xMax][zMax][1] + terrain[xMid][zMid][1] + terrain[xMid][zMax + zMid][1];
        heightDisplacement = heightDisplacement/4;
    }
    randomHeight = rand() % (2*(xMax - xMin));
    randomHeight = randomHeight - (xMax - xMin);
    randomHeight = randomHeight/2;
    terrain[x][z][1] = (randomHeight * MAX_SLOPE) + heightDisplacement;

    if (terrain[x][z][1] < testMinHeight){
        testMinHeight = terrain[x][z][1];
    }
    if (terrain[x][z][1] > testMaxHeight){
        testMaxHeight = terrain[x][z][1];
    }
}
*/



void diamondStep(float*** terrain,int x,int z,int sWidth, float offset) {
    //No need to check bounds because will always be at center of a square with halfWidth = sWidth
    float avg = terrain[x-sWidth][z-sWidth][1] + terrain[x+sWidth][z+sWidth][1] + terrain[x+sWidth][z-sWidth][1] + terrain[x-sWidth][z+sWidth][1];
    avg/=4.0;
    terrain[x][z][1] = avg + offset;
}



/*
midPoint DiamondSquareStep(float*** terrain,int xMin, int zMin, int xMax, int zMax, float &testMaxHeight, float &testMinHeight){
    int x = (xMin + xMax)/2;
    int z = (zMin + zMax)/2;
    float heightDisplacement = terrain[xMin][zMin][1] + terrain[xMin][zMax][1] + 
                                                     terrain[xMax][zMin][1] + terrain[xMax][zMax][1];
    heightDisplacement = heightDisplacement/4;                                                       
    int minDistance = pow((xMax - xMin)/2, 2) + pow((zMax - zMin)/2, 2);
    minDistance = sqrt(minDistance);
    srand(time(NULL));
    float randomHeight = (((float)rand())/(float)RAND_MAX) *(2*minDistance);
    randomHeight = randomHeight - minDistance;
    terrain[x][z][1] = (randomHeight * MAX_SLOPE) + heightDisplacement;

    if (terrain[x][z][1] < testMinHeight){
        testMinHeight = terrain[x][z][1];
    }
    if (terrain[x][z][1] > testMaxHeight){
        testMaxHeight = terrain[x][z][1];
    }

    SquareStep(terrain, xMin, zMin, xMax, zMax, x, z,testMaxHeight,testMinHeight);
    midPoint mid;
    mid.x = x;
    mid.z = z;
    return mid;
}
*/

void diamondSquare(float*** terrain, int width, float rangeFactor) {
    int halfWidth = width/2;

    //base Case
    if(halfWidth < 1) return;

    for(int z = halfWidth; z < Z_SCALE-1; z+=width) {
        for(int x = halfWidth; x < X_SCALE-1; x+=width){
            diamondStep(terrain, x, z, halfWidth, (((float)rand())/(float)RAND_MAX)*2*rangeFactor-rangeFactor);
        }
    }
    for(int z = 0; z <= Z_SCALE-1; z+=halfWidth){
        for(int x = (z+halfWidth)%width; x <= X_SCALE-1; x+= width){
            squareStep(terrain, x, z, halfWidth, (((float)rand())/(float)RAND_MAX)*2*rangeFactor-rangeFactor);
        }
    }
    diamondSquare(terrain, width/2, pow(2,-ROUGH_SCALE)*rangeFactor);
}


/*
void DiamondSquare(float*** terrain,int xMin, int zMin, int xMax, int zMax, int recursiveSteps, float &testMaxHeight, float &testMinHeight){
    if (recursiveSteps > 0){
        midPoint mid = DiamondSquareStep(terrain,xMin,zMin,xMax,zMax, testMaxHeight, testMinHeight);
        //topLeft
        DiamondSquare(terrain,xMin,zMin, mid.x, mid.z, recursiveSteps-1, testMaxHeight, testMinHeight);
        //topRight
        DiamondSquare(terrain, mid.x,zMin, xMax, mid.z, recursiveSteps-1, testMaxHeight, testMinHeight);
        //bottomLeft
        DiamondSquare(terrain, xMin,mid.z, mid.x, zMax, recursiveSteps-1, testMaxHeight, testMinHeight);     
        //bottomRight
        DiamondSquare(terrain, mid.x, mid.z, xMax, zMax, recursiveSteps-1, testMaxHeight, testMinHeight);
    }
}
*/
int GetNumberOfNeighbors(int x, int z){
    int result = 6;
    if ((x == 0 && z == 0) || (x == X_SCALE-1 && z == Z_SCALE-1)){
        result = 2;
    }
    else if ((x == 0 && z == Z_SCALE-1) || (x == X_SCALE-1 && z == 0)){
        result = 3;
    }else if (x == 0 || x == X_SCALE-1 || z == 0 || z == Z_SCALE-1){
        result = 4;
    }
    return result; 
}

int ** neighborFinder(int x, int z, int numberOfNeighbors){
    int **result = new int*[numberOfNeighbors];
    for (int i = 0; i < numberOfNeighbors; i++){
        result[i] = new int[2];
    }

    if (x == 0 && z == 0){
        result[0][0] =  x+1;
        result[0][1] =  z;
        result[1][0] =  x;
        result[1][1] =  z+1;
    }else if (x == 0 && z == Z_SCALE-1){
        result[0][0] =  x;
        result[0][1] =  z-1;
        result[1][0] =  x+1;
        result[1][1] =  z-1;
        result[2][0] =  x+1;
        result[2][1] =  z;
    }else if (x == X_SCALE-1 && z == 0){
        result[0][0] =  x;
        result[0][1] =  z+1;
        result[1][0] =  x-1;
        result[1][1] =  z+1;
        result[2][0] =  x-1;

        result[2][1] =  z;
    }else if (x == X_SCALE-1 && z == Z_SCALE-1){
        result[0][0] =  x-1;
        result[0][1] =  z;
        result[1][0] =  x;
        result[1][1] =  z-1;
    }else if (x == 0){
        result[0][0] =  x;
        result[0][1] =  z-1;
        result[1][0] =  x+1;
        result[1][1] =  z-1;
        result[2][0] =  x+1;
        result[2][1] =  z; 
        result[3][0] =  x;
        result[3][1] =  z+1;       
    }else if (x == X_SCALE-1){
        result[0][0] =  x;
        result[0][1] =  z+1;
        result[1][0] =  x-1;
        result[1][1] =  z+1;
        result[2][0] =  x-1;
        result[2][1] =  z;
        result[3][0] =  x;
        result[3][1] =  z-1;
    }else if (z == 0){
        result[0][0] =  x+1;
        result[0][1] =  z;
        result[1][0] =  x;
        result[1][1] =  z+1;
        result[2][0] =  x-1;
        result[2][1] =  z+1;
        result[3][0] =  x-1;
        result[3][1] =  z; 
    }else if (z == Z_SCALE-1){
        result[0][0] =  x-1;
        result[0][1] =  z;
        result[1][0] =  x;
        result[1][1] =  z-1;
        result[2][0] =  x+1;
        result[2][1] =  z-1;
        result[3][0] =  x+1;
        result[3][1] =  z; 
    }else{
        result[0][0] =  x;
        result[0][1] =  z-1;
        result[1][0] =  x+1;
        result[1][1] =  z-1;
        result[2][0] =  x+1;
        result[2][1] =  z;
        result[3][0] =  x;
        result[3][1] =  z+1;
        result[4][0] =  x-1;
        result[4][1] =  z+1; 
        result[5][0] =  x-1;
        result[5][1] =  z;   
    }

    return result; 
}



void laplacianSmoothing(float *** terrain, int recursion){

    if (recursion > 0){
        float** OriginalTerrain = new float*[X_SCALE];
        for(int i = 0; i < X_SCALE; i++) {
            OriginalTerrain[i] = new float[Z_SCALE];
            for(int j = 0; j < Z_SCALE; j++){
                OriginalTerrain[i][j] = terrain[i][j][1];
            }
        }

        for (int x  = 0; x < X_SCALE; x++){
            for (int z = 0; z < Z_SCALE; z++){
                int numNeighbors = GetNumberOfNeighbors(x,z);
                int **n = neighborFinder(x,z, numNeighbors);
                float total = 0;
                for (int i = 0; i < numNeighbors; i++){
                    total += OriginalTerrain[ n[i][0] ][ n[i][1] ];
                }
                total = (total/numNeighbors) - OriginalTerrain[x][z];

                float lambda = 0.5;
                terrain[x][z][1] = OriginalTerrain[x][z] + (lambda * total);
            }
        }
        recursion--;
        laplacianSmoothing(terrain, recursion);
    }
}

glm::vec3* neighborVectors(float ***terrain, int **n, int numNeighbors){

    glm::vec3 *result = new glm::vec3[numNeighbors];
    int x = 0;
    int z = 0;
    for (int i = 0; i < numNeighbors; i++){
        x = n[i][0];
        z = n[i][1];
        result[i] = glm::vec3(terrain[x][z][0],terrain[x][z][1], terrain[x][z][2]);
    }
    return result;
}

void GenerateNormals(float ***normals, float*** terrain){
    //for each triangle calculate the normal and apply it to each of the vertices it belongs to.
    for (int x = 0; x < X_SCALE; x++){
        for (int z = 0; z < Z_SCALE; z++){
            glm::vec3 curVert(terrain[x][z][0],terrain[x][z][1],terrain[x][z][2]);
            int numNeighbors = GetNumberOfNeighbors(x,z);

            glm::vec3 * neighbor = neighborVectors(terrain, neighborFinder(x,z, numNeighbors), numNeighbors);

            glm::vec3 normal(0.0f,0.0f,0.0f);
            for (int i = 0; i < numNeighbors-1; i++){
                glm::vec3 edge1 = neighbor[i] - curVert;
                glm::vec3 edge2 = neighbor[i+1] - curVert;
                normal += glm::cross(edge2, edge1);
            }
            normal = glm::normalize(normal);
            normals[x][z][0] = normal.x;
            normals[x][z][1] = normal.y;
            normals[x][z][2] = normal.z;
        }
    }

    
}