#include "reader.h"
#include "graphics/scene/scene.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <gdal/cpl_conv.h>
#include <gdal/cpl_error.h>
#include <gdal/gdal_priv.h>
#include <gdal/ogr_core.h>
#include <gdal/ogr_geometry.h>
#include <gdal/ogrsf_frmts.h>
#include <iomanip>
#include <iostream>
#include <libheif/heif.h>
#include <string>
#include <vector>
#include <filesystem>


void GV::initReaderDrivers()
{
  // Register all gdal drivers -- load every possible driver gdal has, vector and raster
  GDALAllRegister();
}

/////////////////////////////////// RasterReader ///////////////////////////////////

GDALDataset* GV::RasterReader::openDataset(const std::string path)
{
  GDALDataset* dataset;
  dataset = (GDALDataset*) GDALOpen(path.c_str(),GA_ReadOnly);
  if (dataset == NULL) throw std::runtime_error("Failed to load the raster dataset");
  return dataset;
}

GV::RasterReader::Dimensions GV::RasterReader::getDimensions(GDALDataset *dataset)
{
  GV::RasterReader::Dimensions dimensions;
  dimensions.width = GDALGetRasterXSize(dataset);
  dimensions.height = GDALGetRasterYSize(dataset);
  return dimensions;
}

std::string GV::RasterReader::getProjection(const GDALDataset *dataset)
{
  return std::string(dataset->GetProjectionRef());
}

float GV::RasterReader::getNoDataValue(GDALDataset* dataset, uint32_t band)
{
  GDALRasterBand* dataBand = dataset -> GetRasterBand(band);
  int pbSuccess = 0;
  double nonDataValue = dataBand -> GetNoDataValue(&pbSuccess);
  // Prevent error in function to check if number is equal to nonDataValue
  if (std::isnan(nonDataValue)) nonDataValue = -9999.0;
  return nonDataValue;
}

void GV::RasterReader::loadData(std::vector<float>& rasterData, GDALDataset* dataset, uint32_t band)
{
  
  uint32_t numBands = dataset -> GetRasterCount();

  GDALRasterBand* dataBand = dataset -> GetRasterBand(band);

  uint32_t width = dataBand -> GetXSize();
  uint32_t height = dataBand -> GetYSize();

  //reserve memory in vector 
  rasterData.reserve(width * height);

  // Something to hold the data --- can be any storaged. 
  float *pafScanline;
  pafScanline = (float *) CPLMalloc(sizeof(float)*width*height);
  
  // RasterIO will load the data
  auto err = dataBand->RasterIO(GF_Read,0,0, width, height, pafScanline,width,height,GDT_Float32,0,0);
  if (err != CE_None)  throw std::runtime_error("Failed to load the raster band");

  // fill with the first value in the upper left corner
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      rasterData.push_back(pafScanline[i * width + j]);
    }
  }

  // Free the pafScanline buffer when it is not longer used
  CPLFree(pafScanline);
}


void GV::RasterReader::getHDF5CellCoordinates(GDALDataset* dataset, std::vector<std::string>& cellCoordinatesName, std::vector<std::string>& cellCoordinatesPath)
{
  char** metaData = dataset -> GetMetadata("SUBDATASETS");
  int i = 0;
  while (metaData[i] != NULL) 
  {
    std::string str = metaData[i];
    if (str.find("NAME") != std::string::npos)
    {
      if (str.find("Cells_Center_Coordinate") != std::string::npos)
      {
	int pos = str.find("=");
	cellCoordinatesPath.push_back(str.substr(pos + 1));
	std::filesystem::path fakePath(str.substr(pos+1));
	cellCoordinatesName.push_back(fakePath.parent_path().filename().string());
      }
    }
    i += 1;
  }
}

void GV::RasterReader::getHDF5Datasets(GDALDataset* dataset, std::vector<std::string>& summaryDatasetsName, std::vector<std::string>& timeSeriesDatasetsName, std::vector<std::string>& summaryDatasetsPath, std::vector<std::string>& timeSeriesDatasetsPath, const std::string& flowAreaName)
{
  char** metaData = dataset -> GetMetadata("SUBDATASETS");
  int i = 0;
  while (metaData[i] != NULL)
  {
    std::string str = metaData[i];
    if (str.find("NAME") != std::string::npos)
    {
      int pos = str.find("=");
      if (str.find("Summary_Output") != std::string::npos)
      {
	std::filesystem::path fakePath(str.substr(pos+1));
	std::string flowArea = fakePath.parent_path().filename().string();
	if (flowArea != flowAreaName) continue;
	// Push all the path of the HDF5 dataset
	summaryDatasetsPath.push_back(str.substr(pos + 1));
	std::string value = fakePath.filename().string() + " (Summary Dataset)";
	summaryDatasetsName.push_back(value);
      }
      else if (str.find("Unsteady_Time_Series") != std::string::npos && str.find("Boundary_Conditions") == std::string::npos)
      {
	std::filesystem::path fakePath(str.substr(pos+1));
	std::string flowArea = fakePath.parent_path().filename().string();
	if (flowArea != flowAreaName) continue;
	// Push all the path of the HDF5 dataset
	timeSeriesDatasetsPath.push_back(str.substr(pos + 1));
	std::string value = fakePath.filename().string() + " (Time Series Dataset)";
	timeSeriesDatasetsName.push_back(value);
      }
    }
    i+= 1;
  }
}

void GV::RasterReader::loadHDF5Dataset(GDALDataset* coordinatesDataset, GDALDataset* valuesDataset, uint32_t coordinatesBand, uint32_t valuesBand)
{
  //char** metaData = dataset -> GetMetadata("");
  int i = 0;
  /*while (metaData[i] != NULL)
  {
    std::cout << metaData[i] << "\n";


    i += 1;
  }*/
  GDALRasterBand* coordinatesDataband = coordinatesDataset -> GetRasterBand(coordinatesBand);
  GDALRasterBand* valuesDataband = coordinatesDataset -> GetRasterBand(coordinatesBand);

  uint32_t coordinatesWidth = coordinatesDataband -> GetXSize();
  uint32_t coordinatesHeight = coordinatesDataband -> GetYSize();
  
  uint32_t valuesWidth = valuesDataband -> GetXSize();
  uint32_t valuesHeight = valuesDataband -> GetYSize();
  std::cout << coordinatesWidth << "  " << coordinatesHeight << "\n";
  std::cout << valuesWidth << "  " << valuesHeight << "\n";

  std::cout << "DATA HEC" << "\n";
  std::cout << valuesDataset -> GetRasterBand(1) ->GetNoDataValue() << "\n";
  std::cout << valuesDataset -> GetRasterXSize() << "\n";
  std::cout << valuesDataset -> GetRasterYSize() << "\n";
  std::cout << valuesDataset -> GetRasterCount() << "\n\n";

}

void GV::RasterReader::loadImage(GDALDataset *dataset)
{
  int numbands = dataset->GetRasterCount();
}

inline float GV::RasterReader::extractHeightPoint(const std::vector<float>& rasterHeightData, double xPoint, double yPoint, double xOrigin, double yOrigin, float pixelWidth, float pixelHeight, unsigned int widthRaster)
{
  unsigned int xOffset = (int)((xPoint - xOrigin)/pixelWidth); 
  unsigned int yOffset = (int)((yOrigin - yPoint)/pixelHeight);
  return rasterHeightData[xOffset + yOffset * widthRaster];
}

/////////////////////////////////// VectorReader ///////////////////////////////////

GDALDataset* GV::VectorReader::openDataset(std::string path)
{
  GDALDataset* dataset;
  dataset = (GDALDataset*) GDALOpenEx(path.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL );

  if (dataset == NULL) throw std::runtime_error("Failed to load the vector dataset");

  return dataset;
}

GV::VectorReader::GeometryType GV::VectorReader::getGeometryType(GDALDataset *dataset, unsigned int layerPos)
{
  OGRLayer* layer = dataset -> GetLayer(layerPos);
  OGRwkbGeometryType geometryType = layer -> GetGeomType();

  switch (geometryType)
  {
    case wkbPoint:
      return GV::VectorReader::GeometryType::Point;
    case wkbLineString:
      return GV::VectorReader::GeometryType::Line;
    case wkbPolygon:
      return GV::VectorReader::GeometryType::Polygon;
    default:
      throw std::runtime_error("Geometry type not supported");  
  }
}

void GV::VectorReader::getMeanValuesCoordinates(GDALDataset* dataset, MeanValuesCoordinates& meanValuesCoordinates, GeometryType geometryType, const std::vector<float>& rasterHeightData, const uint32_t layerPos, bool hasName, const std::string name, double xOriginRaster, double yOriginRaster, float pixelWidth, float pixelHeight, const unsigned int widthRaster)
{
  OGRLayer* layer;
  if (hasName) layer = dataset -> GetLayerByName(name.c_str());
  else layer = dataset->GetLayer(layerPos);

  OGRGeometry* geometry;
  GV::VectorReader::CountTypesGeometry countTypesGeometry;
  for (auto& feature : layer)
  {
    geometry = feature -> GetGeometryRef();
    if (geometry != NULL && wkbFlatten(geometry -> getGeometryType()) == wkbPoint)
    {
      OGRPoint* point = (OGRPoint*)geometry;
      double x = point -> getX();
      double y = point -> getY();
      float heightPoint = GV::RasterReader::extractHeightPoint(rasterHeightData, x, y, xOriginRaster, yOriginRaster, pixelWidth, pixelHeight, widthRaster); 
      meanValuesCoordinates.meanXPoints += x;
      meanValuesCoordinates.meanZPoints += y;
      meanValuesCoordinates.MeanHeightPoints += heightPoint;
      countTypesGeometry.countPoint += 1;
    }
  }
  meanValuesCoordinates.meanXPoints /= countTypesGeometry.countPoint;
  meanValuesCoordinates.meanZPoints /= countTypesGeometry.countPoint;
  meanValuesCoordinates.MeanHeightPoints /= countTypesGeometry.countPoint;
}

void GV::VectorReader::loadData(std::vector<float>& pointData, GDALDataset *dataset, const std::vector<float>& rasterHeightData, const GV::VectorReader::MeanValuesCoordinates& meanValuesCoordinates, GV::VectorReader::CountTypesGeometry& countTypesGeometry, uint32_t layerPos, bool hasName, std::string name, double xOriginRaster, double yOriginRaster, float pixelWidth, float pixelHeight, const unsigned int widthRaster, const float rasterMeanHeight)
{
  OGRLayer* layer;
  if (hasName) layer = dataset -> GetLayerByName(name.c_str());
  else layer = dataset->GetLayer(layerPos);

  OGRGeometry* geometry;
  for (auto& feature : layer)
  {
    geometry = feature -> GetGeometryRef();
    if (geometry != NULL && wkbFlatten(geometry -> getGeometryType()) == wkbPoint)
    {
      OGRPoint* point = (OGRPoint*)geometry;
      double x = point -> getX();
      double y = point -> getY();
      // IT is necessary to shift the values to locate the points in the corrects coordinates used iwht respect the terrains
      pointData.push_back(x - meanValuesCoordinates.meanXPoints);
      // the height value is in the y axis
      //  The mean vlaue is extracted beacuse it is added with the transform
     
      float heightPoint = GV::RasterReader::extractHeightPoint(rasterHeightData, x, y, xOriginRaster, yOriginRaster, pixelWidth, pixelHeight, widthRaster); 
      pointData.push_back(heightPoint - meanValuesCoordinates.MeanHeightPoints);
      pointData.push_back(meanValuesCoordinates.meanZPoints - y);
      countTypesGeometry.countPoint += 1; 
    }

    else if (geometry != NULL && wkbFlatten(geometry -> getGeometryType()) == wkbLineString)
    {
      countTypesGeometry.countLineString += 1;
    }

    else if (geometry != NULL && wkbFlatten(geometry -> getGeometryType()) == wkbPolygon)
    {
      countTypesGeometry.countPolygon += 1;
    }

  } 
}

void GV::VectorReader::getCountGeometry(const std::filesystem::path& pathData, GV::VectorReader::CountTypesGeometry& countTypesGeometry, const int layerPos, bool hasName, const std::string name)
{
  GDALDataset* dataset = GV::VectorReader::openDataset(pathData);
  OGRLayer* layer;
  if (hasName) layer = dataset -> GetLayerByName(name.c_str());
  else layer = dataset->GetLayer(layerPos);

  OGRGeometry* geometry;
  for (auto& feature : layer)
  {
    geometry = feature -> GetGeometryRef();
    if (geometry != NULL && wkbFlatten(geometry -> getGeometryType()) == wkbPoint) countTypesGeometry.countPoint += 1;
    else if (geometry != NULL && wkbFlatten(geometry -> getGeometryType()) == wkbLineString) countTypesGeometry.countLineString += 1;

    else if (geometry != NULL && wkbFlatten(geometry -> getGeometryType()) == wkbPolygon) countTypesGeometry.countPolygon += 1;
  } 

}
