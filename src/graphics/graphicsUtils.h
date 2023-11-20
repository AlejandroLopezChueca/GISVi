#pragma once

#include <cstdint>
#include <vector>
namespace GV
{
  namespace GraphicsUtils
  {
    /* @brief Create a batch of cubes from points with its elevations equal to a MDT. Each cube has 8 vertices and 36 indexes.
     *
     *
     *
     *
     */
    void BatchCubesFromPoints(std::vector<float>& pointCoordinates,std::vector<float>& cubesCoordinates, std::vector<unsigned int>& indexCubes, float xWidthCube, float zWidthCube, float heightCube, float& meanHeightCubes);
    
    /* @brief Create instances of cubes from points with its elevations equal to a MDT. Each cube has 8 vertices and 36 indexes.
     *
     *
     *
     *
     */
    void instanceCubesFromPoints(std::vector<float>& pointsCoordinates, std::vector<float>& cubesCoordinates, std::vector<float>& indexCubes, uint32_t instanceCount,float xWidthCube, float zWidthCube, float heightCube);
  }
}
