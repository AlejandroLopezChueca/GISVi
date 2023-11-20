#include <cstdint>
#include <iostream>
#include <vector>

#include "graphicsUtils.h"

void GV::GraphicsUtils::BatchCubesFromPoints(std::vector<float>& pointsCoordinates, std::vector<float>& cubesCoordinates, std::vector<unsigned int>& indexCubes, float xWidthCube, float zWidthCube, float heightCube, float& meanHeightCubes)
{
  // There are 24 floats per cube
  cubesCoordinates.reserve(24 * pointsCoordinates.size()/3);
  // There are 36 indexes per cube
  indexCubes.reserve(36 * pointsCoordinates.size()/3);
  int cont = 0;
  // the coordinates are always 3D
  // The x coordinate is the x component of each point
  // The y coordinate is the z component of each point
  // The z coordinate is the y component of each point
  for (int i = 0; i < pointsCoordinates.size(); i += 3)
  {

    // For the y coordinate is substracted height/2 to place correctly the gizmo in the center of the cube. This substraction is taken into account inn the transform

    int idx = cont * 24;
    // Vertex 0
    cubesCoordinates.push_back(pointsCoordinates[i] - xWidthCube/2);
    cubesCoordinates.push_back(pointsCoordinates[i + 1] - heightCube/2.0f);
    cubesCoordinates.push_back(pointsCoordinates[i + 2] + zWidthCube/2);
    
    // Vertex 1
    cubesCoordinates.push_back(pointsCoordinates[i] + xWidthCube/2);
    cubesCoordinates.push_back(pointsCoordinates[i + 1] - heightCube/2.0f);
    cubesCoordinates.push_back(pointsCoordinates[i + 2] + zWidthCube/2);

    // Vertex 2
    cubesCoordinates.push_back(cubesCoordinates[idx + 3]);
    cubesCoordinates.push_back(cubesCoordinates[idx + 1] + heightCube);
    cubesCoordinates.push_back(cubesCoordinates[idx + 2]);
    
    // Vertex 3
    cubesCoordinates.push_back(cubesCoordinates[idx]);
    cubesCoordinates.push_back(cubesCoordinates[idx + 4] + heightCube);
    cubesCoordinates.push_back(cubesCoordinates[idx + 5]);

    // Vertex 4
    cubesCoordinates.push_back(cubesCoordinates[idx]);
    cubesCoordinates.push_back(cubesCoordinates[idx + 1]);
    cubesCoordinates.push_back(cubesCoordinates[idx + 2] - zWidthCube);
    
    // Vertex 5
    cubesCoordinates.push_back(cubesCoordinates[idx + 3]); 
    cubesCoordinates.push_back(cubesCoordinates[idx + 4]);
    cubesCoordinates.push_back(cubesCoordinates[idx + 5] - zWidthCube);

    // Vertex 6
    cubesCoordinates.push_back(cubesCoordinates[idx + 15]); 
    cubesCoordinates.push_back(cubesCoordinates[idx + 16] + heightCube); 
    cubesCoordinates.push_back(cubesCoordinates[idx + 17]);

    // Vertex7
    cubesCoordinates.push_back(cubesCoordinates[idx + 12]);
    cubesCoordinates.push_back(cubesCoordinates[idx + 13] + heightCube);
    cubesCoordinates.push_back(cubesCoordinates[idx + 14]);

    // There are 8 unique indexes per cube
    unsigned int idx2 = cont * 8;
    // front plane xy
    indexCubes.push_back(idx2);
    indexCubes.push_back(idx2 + 1);
    indexCubes.push_back(idx2 + 2);
    indexCubes.push_back(idx2 + 2);
    indexCubes.push_back(idx2);
    indexCubes.push_back(idx2 + 3);

    //left side, plane yz
    indexCubes.push_back(idx2 + 3);
    indexCubes.push_back(idx2);
    indexCubes.push_back(idx2 + 4);
    indexCubes.push_back(idx2 + 4);
    indexCubes.push_back(idx2 + 3);
    indexCubes.push_back(idx2 + 7);

    //back, plane xy
    indexCubes.push_back(idx2 + 7);
    indexCubes.push_back(idx2 + 6);
    indexCubes.push_back(idx2 + 4);
    indexCubes.push_back(idx2 + 4);
    indexCubes.push_back(idx2 + 5);
    indexCubes.push_back(idx2 + 6);

    // up, plane xz
    indexCubes.push_back(idx2 + 6);
    indexCubes.push_back(idx2 + 7);
    indexCubes.push_back(idx2 + 3);
    indexCubes.push_back(idx2 + 3);
    indexCubes.push_back(idx2 + 2);
    indexCubes.push_back(idx2 + 6);

    // right, plane yz
    indexCubes.push_back(idx2 + 6);
    indexCubes.push_back(idx2 + 2);
    indexCubes.push_back(idx2 + 5);
    indexCubes.push_back(idx2 + 5);
    indexCubes.push_back(idx2 + 2);
    indexCubes.push_back(idx2 + 1);

    // down plane xz
    indexCubes.push_back(idx2 + 1);
    indexCubes.push_back(idx2 + 5);
    indexCubes.push_back(idx2);
    indexCubes.push_back(idx2);
    indexCubes.push_back(idx2 + 5);
    indexCubes.push_back(idx2 + 4);
    cont += 1;

    // mean values
    meanHeightCubes += (pointsCoordinates[i + 1] + heightCube)/2;
  }
  meanHeightCubes /= ((float)pointsCoordinates.size()/3.0f);
}

void GV::GraphicsUtils::instanceCubesFromPoints(std::vector<float>& pointsCoordinates, std::vector<float>& cubesCoordinates, std::vector<float>& indexCubes, uint32_t instanceCount, float xWidthCube, float zWidthCube, float heightCube)
{

  for (int i = 0; i < instanceCount; i++)
  {

  }

}
