#pragma once

#include "graphics/scene/scene.h"

#include <cstdint>
#include <filesystem>
#include <gdal/gdal.h>
#include <gdal/gdal_priv.h>
#include <gdal/ogrsf_frmts.h>

#include <string>
#include <vector>

namespace GV 
{
  void initReaderDrivers();

  namespace RasterReader
  {
    void init();
    GDALDataset* openDataset(const std::string path);

    // number of pixel in each direction
    struct Dimensions
    {
      unsigned int width, height;
    };

    Dimensions getDimensions(GDALDataset* dataset);
    std::string getProjection(const GDALDataset* dataset);

    float getNoDataValue(GDALDataset* dataset, uint32_t band = 1);
 
    void loadData(std::vector<float>& rasterData, GDALDataset* dataset, uint32_t band = 1);

    void getHDF5CellCoordinates(GDALDataset* dataset, std::vector<std::string>& cellCoordinatesName, std::vector<std::string>& cellCoordinatesPath);

    void getHDF5Datasets(GDALDataset* dataset, std::vector<std::string>& summaryDatasetsName, std::vector<std::string>& timeSeriesDatasetsName, std::vector<std::string>& summaryDatasetsPath, std::vector<std::string>& timeSeriesDatasetsPath, const std::string& flowAreaName);

    void loadHDF5Dataset(GDALDataset* coordinatesDataset, GDALDataset* valuesDataset, uint32_t coordinatesBand = 1, uint32_t valuesBand = 1);

    void loadImage(GDALDataset* dataset);

    void fetchWMSImage();

    /* @brieg EXtract the height value from a raster in a point. The point and the raster layer have to be in the same reference system
     *
     *
     *
     * */

    inline float extractHeightPoint(const std::vector<float>& rasterHeightData, double xPoint, double yPoint, double xOrigin, double yOrigin, float pixelWidth, float pixelHeight, unsigned int widthRaster);
  }

  namespace VectorReader
  {
    enum class GeometryType
    {
      Point = 0, Line, Polygon
    };

    GDALDataset* openDataset(std::string path);

    GeometryType getGeometryType(GDALDataset* dataset, unsigned int layerPos);

    // Mean values in world coordinates
    struct MeanValuesCoordinates
    {
      double meanXPoints = 0.0;
      double MeanHeightPoints = 0.0;
      double meanZPoints = 0.0;
    };

    void getMeanValuesCoordinates(GDALDataset* dataset, MeanValuesCoordinates& meanValuesCoordinates, GeometryType geometryType, const std::vector<float>& rasterHeightData, const uint32_t layerPos = 0, bool hasName = false, const std::string name = "", double xOriginRaster = 0, double yOriginRaster = 0, float pixelWidth = 0, float pixelHeight = 0, const unsigned int widthRaster = 0);

    struct CountTypesGeometry
    {
      uint32_t countPoint = 0;
      uint32_t countLineString = 0;
      uint32_t countPolygon = 0;
    };

    void loadData(std::vector<float>& pointData, GDALDataset* dataset, const std::vector<float>& rasterHeightData, const MeanValuesCoordinates& meanValuesCoordinates, CountTypesGeometry& countTypesGeometry, uint32_t layerPos = 0, bool hasName = false, std::string name = "", double xOriginRaster = 0, double yOriginRaster = 0, float pixelWidth = 0, float pixelHeight = 0, const unsigned int widthRaster = 0, const float rasterMeanHeight = 0.0f);

    void getCountGeometry(const std::filesystem::path& pathData, CountTypesGeometry& countTypesGeometry, const int layerPos = 0, bool hasName = false, const std::string name = "");
  }
}
