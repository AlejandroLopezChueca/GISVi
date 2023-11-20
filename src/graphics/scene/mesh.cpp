#include "mesh.h"
#include "components.h"
#include "graphics/API.h"
#include "graphics/buffer.h"
#include "graphics/graphicsUtils.h"
#include "graphics/shader.h"
#include "graphics/vertexArray.h"
#include "graphics/renderer.h"
#include "io/reader.h"
#include "scene.h"
#include "math/math.h"

#include "glm/fwd.hpp"
#include "glm/gtx/quaternion.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <functional>
#include <gdal/gdal_priv.h>
#include <gdal/ogr_core.h>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

///////////////////////////////////// Mesh /////////////////////////////////////

std::vector<glm::mat4> GV::Mesh::calculateTransformInstanceData()
{
  // Default values
  glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
  glm::vec3 scale = {1.0f, 1.0f, 1.0f};
  glm::mat4 identity = glm::mat4(1.0f);

  std::vector<glm::mat4> transformData;
  for (int i = 0; i < m_InstanceCount; i++)
  {
    glm::mat4 rotationMatrix = glm::toMat4(glm::quat(rotation));
    transformData.push_back(glm::translate(identity, m_TranslationData[i]) * rotationMatrix * glm::scale(identity, scale));
  }
  return transformData;
}


///////////////////////////////////// CubeMesh /////////////////////////////////////

GV::CubeMesh::CubeMesh(const std::string& pathData, const std::vector<float>& rasterHeightData, const std::array<double, 6>& rasterGeoTransform, GV::ShiftScaleValues& shiftScaleValues, const GV::CubeMeshParameters& parameters, const bool setMesh)
  : m_XWidthCube(parameters.xWidthCube), m_ZWidthCube(parameters.zWidthCube), m_HeightCube(parameters.heightCube)
{
  getVertexDataFromPoints(pathData, rasterHeightData, rasterGeoTransform, shiftScaleValues,parameters);
  if (setMesh) setupMesh(parameters);
}

void GV::CubeMesh::getVertexDataFromPoints(const std::string& pathPoints, const std::vector<float>& rasterHeightData, const std::array<double, 6>& rasterGeoTransform, GV::ShiftScaleValues& shiftScaleValues, const GV::CubeMeshParameters& parameters)
{
  GDALDataset* dataset = GV::VectorReader::openDataset(pathPoints);
  double XSizePixel = rasterGeoTransform[1]; //w-e pixel resolution / pixel width.
  double YSizePixel = rasterGeoTransform[5]; //n-s pixel resolution / pixel height
  // negative value for a north-up image
  if (YSizePixel < 0) YSizePixel *= -1;
  if (XSizePixel < 0) XSizePixel *= -1; 

  double xOriginRaster = rasterGeoTransform[0];
  double yOriginRaster = rasterGeoTransform[3];
  
  // It is supposed that there is only one layer in the dataset
  unsigned int layerPos = 0;
  bool hasName = false;
  std::string nameLayer = "";
  GV::VectorReader::CountTypesGeometry countTypesGeometry;
  
  // Mean values coordinates points
  GV::VectorReader::MeanValuesCoordinates meanValuesCoordinates;
  GV::VectorReader::GeometryType geometryType = GV::VectorReader::GeometryType::Point;
  GV::VectorReader::getMeanValuesCoordinates(dataset, meanValuesCoordinates, geometryType, rasterHeightData, layerPos, hasName, nameLayer, xOriginRaster, yOriginRaster, XSizePixel, YSizePixel, parameters.rasterWidth);

  // values to shift the cubes with the transform, when there is only one tranform for all the cubes
  m_XValueShift = meanValuesCoordinates.meanXPoints - shiftScaleValues.xShift;
  m_ZValueShift = shiftScaleValues.yShift - meanValuesCoordinates.meanZPoints;
  m_HeightValueShift = meanValuesCoordinates.MeanHeightPoints + parameters.heightCube/2.0f;
  
  std::vector<float> pointData; // coordinates in local world
  GV::VectorReader::loadData(pointData, dataset, rasterHeightData, meanValuesCoordinates, countTypesGeometry, layerPos, hasName, nameLayer, xOriginRaster, yOriginRaster, XSizePixel, YSizePixel, parameters.rasterWidth, parameters.rasterMeanHeight);

  m_InstanceCount = countTypesGeometry.countPoint;

  switch (parameters.meshTypeCreation) 
  {
    case GV::MeshTypeCreation::Batching:
      batchCubesFromPoints(pointData, parameters);
    case GV::MeshTypeCreation::Instancing:
      instanceCubesFromPoints(pointData, parameters, shiftScaleValues, meanValuesCoordinates); 
      break; 
  }
  std::cout << "VERTEX DATA\n";
  for (int i = 0; i < m_VertexData.size(); i+= 3)
  {
    std::cout << m_VertexData[i] <<"  ";
    std::cout << m_VertexData[i+1] <<"  ";
    std::cout << m_VertexData[i+2] <<"\n";
  }
  std::cout << "OFFSET DATA\n";
  for (int i = 0; i < m_OffsetData.size(); i+= 3)
  {
    std::cout << m_OffsetData[i] <<"  ";
    std::cout << m_OffsetData[i+1] <<"  ";
    std::cout << m_OffsetData[i+2] <<"\n";
  }
}

void GV::CubeMesh::batchCubesFromPoints(std::vector<float>& pointsCoordinates, const CubeMeshParameters& parameters)
{
  // There are 24 floats per cube
  m_VertexData.reserve(24 * pointsCoordinates.size()/3);
  // There are 36 indexes per cube
  m_IndexData.reserve(36 * pointsCoordinates.size()/3);
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
    m_VertexData.push_back(pointsCoordinates[i] - parameters.xWidthCube/2.0f);
    m_VertexData.push_back(pointsCoordinates[i + 1] - parameters.heightCube/2.0f);
    m_VertexData.push_back(pointsCoordinates[i + 2] + parameters.zWidthCube/2.0f);
    
    // Vertex 1
    m_VertexData.push_back(pointsCoordinates[i] + parameters.xWidthCube/2.0f);
    m_VertexData.push_back(pointsCoordinates[i + 1] - parameters.heightCube/2.0f);
    m_VertexData.push_back(pointsCoordinates[i + 2] + parameters.zWidthCube/2.0f);

    // Vertex 2
    m_VertexData.push_back(m_VertexData[idx + 3]);
    m_VertexData.push_back(m_VertexData[idx + 1] + parameters.heightCube);
    m_VertexData.push_back(m_VertexData[idx + 2]);
    
    // Vertex 3
    m_VertexData.push_back(m_VertexData[idx]);
    m_VertexData.push_back(m_VertexData[idx + 4] + parameters.heightCube);
    m_VertexData.push_back(m_VertexData[idx + 5]);

    // Vertex 4
    m_VertexData.push_back(m_VertexData[idx]);
    m_VertexData.push_back(m_VertexData[idx + 1]);
    m_VertexData.push_back(m_VertexData[idx + 2] - parameters.zWidthCube);
    
    // Vertex 5
    m_VertexData.push_back(m_VertexData[idx + 3]); 
    m_VertexData.push_back(m_VertexData[idx + 4]);
    m_VertexData.push_back(m_VertexData[idx + 5] - parameters.zWidthCube);

    // Vertex 6
    m_VertexData.push_back(m_VertexData[idx + 15]); 
    m_VertexData.push_back(m_VertexData[idx + 16] + parameters.heightCube); 
    m_VertexData.push_back(m_VertexData[idx + 17]);

    // Vertex7
    m_VertexData.push_back(m_VertexData[idx + 12]);
    m_VertexData.push_back(m_VertexData[idx + 13] + parameters.heightCube);
    m_VertexData.push_back(m_VertexData[idx + 14]);

    // There are 8 unique indexes per cube
    unsigned int idx2 = cont * 8;
    // front plane xy
    m_IndexData.push_back(idx2);
    m_IndexData.push_back(idx2 + 1);
    m_IndexData.push_back(idx2 + 2);
    m_IndexData.push_back(idx2 + 2);
    m_IndexData.push_back(idx2);
    m_IndexData.push_back(idx2 + 3);

    //left side, plane yz
    m_IndexData.push_back(idx2 + 3);
    m_IndexData.push_back(idx2);
    m_IndexData.push_back(idx2 + 4);
    m_IndexData.push_back(idx2 + 4);
    m_IndexData.push_back(idx2 + 3);
    m_IndexData.push_back(idx2 + 7);

    //back, plane xy
    m_IndexData.push_back(idx2 + 7);
    m_IndexData.push_back(idx2 + 6);
    m_IndexData.push_back(idx2 + 4);
    m_IndexData.push_back(idx2 + 4);
    m_IndexData.push_back(idx2 + 5);
    m_IndexData.push_back(idx2 + 6);

    // up, plane xz
    m_IndexData.push_back(idx2 + 6);
    m_IndexData.push_back(idx2 + 7);
    m_IndexData.push_back(idx2 + 3);
    m_IndexData.push_back(idx2 + 3);
    m_IndexData.push_back(idx2 + 2);
    m_IndexData.push_back(idx2 + 6);

    // right, plane yz
    m_IndexData.push_back(idx2 + 6);
    m_IndexData.push_back(idx2 + 2);
    m_IndexData.push_back(idx2 + 5);
    m_IndexData.push_back(idx2 + 5);
    m_IndexData.push_back(idx2 + 2);
    m_IndexData.push_back(idx2 + 1);

    // down plane xz
    m_IndexData.push_back(idx2 + 1);
    m_IndexData.push_back(idx2 + 5);
    m_IndexData.push_back(idx2);
    m_IndexData.push_back(idx2);
    m_IndexData.push_back(idx2 + 5);
    m_IndexData.push_back(idx2 + 4);
    cont += 1;

    // mean values
    m_MeanHeight += (pointsCoordinates[i + 1] + parameters.heightCube)/2.0f;
    m_MeanZCoordinate += (pointsCoordinates[i + 2] + parameters.zWidthCube)/2.0f;
  }
  m_MeanHeight /= ((float)pointsCoordinates.size()/3.0f);
  m_MeanZCoordinate /= ((float)pointsCoordinates.size()/3.0f);
}

void GV::CubeMesh::instanceCubesFromPoints(std::vector<float>& pointsCoordinates, const CubeMeshParameters& parameters, const GV::ShiftScaleValues& shiftScaleValues, const GV::VectorReader::MeanValuesCoordinates& meanValuesCoordinates)
{
  // Init Vertex data for the first instance
  std::vector<float> pointDataFirstInstance;
  if (parameters.typeInstancing == GV::TypeInstancing::MultiEntity) pointDataFirstInstance = {0, 0, 0};
  else pointDataFirstInstance = {pointsCoordinates.begin(), pointsCoordinates.begin() + 3};
  batchCubesFromPoints(pointDataFirstInstance, parameters);
   
  switch (parameters.typeInstancing) 
  {
    case GV::TypeInstancing::MonoEntity:
    {
      // There are 3 floats per instance
      m_OffsetData.reserve(3 * m_InstanceCount);
      // For the first instance the offset is 0
      m_OffsetData.push_back(0.0f);
      m_OffsetData.push_back(0.0f);
      m_OffsetData.push_back(0.0f);
      for (int i = 3; i < m_InstanceCount * 3; i += 3)
      {
	// Difference between the point of the instance and the first instance
	m_OffsetData.push_back(pointsCoordinates[i] - pointsCoordinates[0]);
	m_OffsetData.push_back(pointsCoordinates[i + 1] - pointsCoordinates[1]);
	m_OffsetData.push_back(pointsCoordinates[i + 2] - pointsCoordinates[2]);	
      }
    }
    case GV::TypeInstancing::MultiEntity:
    {
      for (int i = 0; i < m_InstanceCount * 3; i += 3)
      {	
	// values to shift the cubes with the transform
	// sum meanValues to rstore coordinates to global world
	double xValueShift = pointsCoordinates[i] + meanValuesCoordinates.meanXPoints - shiftScaleValues.xShift;
	double heightValueShift = pointsCoordinates[i + 1] + meanValuesCoordinates.MeanHeightPoints + parameters.heightCube/2.0f;
	double zValueShift = pointsCoordinates[i +2] - meanValuesCoordinates.meanZPoints + shiftScaleValues.yShift;
	m_TranslationData.push_back({xValueShift, heightValueShift, zValueShift});
      }
    } 
  };
}

void GV::CubeMesh::setupMesh(const GV::CubeMeshParameters& parameters)
{
  m_VertexArray = GV::VertexArray::create(parameters.api);
  m_VertexBuffer = GV::VertexBuffer::create(parameters.api, m_VertexData.data(), sizeof(float) * m_VertexData.size(), parameters.bufferTypeDraw);

  GV::BufferLayout layout;
  switch (parameters.meshTypeCreation) 
  {
    case GV::MeshTypeCreation::Batching:
    {
      layout = {{GV::ShaderDataType::Float3, "a_Pos"}};
      m_VertexBuffer -> setLayout(layout);
      m_VertexArray -> addVertexBuffer(m_VertexBuffer.get());
      break;
    }
    case GV::MeshTypeCreation::Instancing:
    {
      switch (parameters.typeInstancing) 
      {
	case GV::TypeInstancing::MonoEntity:
	{
	  layout = {{GV::ShaderDataType::Float3, "a_Pos"}};
	  m_VertexBuffer -> setLayout(layout);
	  m_VertexArray -> addVertexBuffer(m_VertexBuffer.get());

	  m_OffsetBuffer = GV::VertexBuffer::create(parameters.api, m_OffsetData.data(), sizeof(float) * m_OffsetData.size(), parameters.bufferTypeDraw);
	  layout = {{GV::ShaderDataType::Float3, "a_Offset", true}};
	  m_OffsetBuffer -> setLayout(layout);
	  m_VertexArray -> addVertexBuffer(m_OffsetBuffer.get());
	  break;
	}
	case GV::TypeInstancing::MultiEntity:
	{
	  layout = {{GV::ShaderDataType::Float3, "a_Pos"}};
	  m_VertexBuffer -> setLayout(layout);
	  m_VertexArray -> addVertexBuffer(m_VertexBuffer.get());

	  std::vector<glm::mat4> transformData = calculateTransformInstanceData();
	  m_TransformBuffer = GV::VertexBuffer::create(parameters.api, transformData.data(), sizeof(glm::mat4) * transformData.size(), parameters.bufferTypeDraw);
	  layout = {{GV::ShaderDataType::Mat4, "a_Transform", true}};
	  m_TransformBuffer -> setLayout(layout);
	  m_VertexArray -> addVertexBuffer(m_TransformBuffer.get());
	  break; 
	}
      };
    }
  };
  m_IndexBuffer = GV::IndexBuffer::create(parameters.api, m_IndexData.data(), m_IndexData.size());
  m_VertexArray -> setIndexBuffer(m_IndexBuffer.get());
}

///////////////////////////////////// TerrainMesh /////////////////////////////////////

GV::TerrainMesh::TerrainMesh(GV::API api, GV::TypeTessellation typeTessellation, const std::string& path, GV::ShiftScaleValues& shiftScaleValues, GV::BufferTypeDraw bufferTypeDraw)
  :m_TypeTessellation(typeTessellation), m_Path(path)
{
  getVertexData(path, shiftScaleValues);
  setupMesh(api, bufferTypeDraw);
} 

void GV::TerrainMesh::getVertexData(std::string path, GV::ShiftScaleValues& shiftScaleValues)
{
  GDALDataset* dataset = GV::RasterReader::openDataset(path);

  m_NoDataValue = GV::RasterReader::getNoDataValue(dataset);
  std::cout << "NO DATA VALUE : " <<m_NoDataValue << "\n";

  GV::RasterReader::loadData(m_HeightData, dataset, 1);
  GV::RasterReader::Dimensions dimensions = GV::RasterReader::getDimensions(dataset);

  unsigned int height = dimensions.height;
  unsigned int width = dimensions.width;

  m_RasterWidth = width;

  double geoTransform[6];
  // get geotransform of the raster layer
  dataset -> GetGeoTransform(geoTransform);

  for (int i = 0; i < 6; i++) m_GeoTransform[i] = geoTransform[i]; 

  double XSizePixel = m_GeoTransform[1]; //w-e pixel resolution / pixel width.
  double YSizePixel = m_GeoTransform[5]; //n-s pixel resolution / pixel height
  // negative value for a north-up image
  if (YSizePixel < 0) YSizePixel = -YSizePixel;

  // length in length units used int he raster
  double lengthX = width * XSizePixel;
  double lengthY = height * YSizePixel;
   
  // Factor to shift the  terrain meshes. It accounts the difference between the center of the mesh with repsect the mesh used as the origen
  double xShift = 0.0;
  double yShift = 0.0;

  if (shiftScaleValues.hasValues)
  {
    xShift = (m_GeoTransform[0] + lengthX/2.0) - shiftScaleValues.xShift;
    yShift = shiftScaleValues.yShift - (m_GeoTransform[3] - lengthY/2.0);
  }

  // mean values
  // This values have to be substracted in the construction of the vertex data because they are added later in the transform
  m_MeanHeight = GV::Math::getMeanValueRaster(m_HeightData, m_NoDataValue);
  m_MeanXCoordinate = (-lengthX + (width - 1) * XSizePixel)/2.0 + xShift;
  m_MeanZCoordinate = (-lengthY + (height - 1) * YSizePixel)/2.0 + yShift;
  std::cout << "MEAN HEIGHT : " <<m_MeanHeight << "\n";
      static int a = 0;
      a+=1;

  switch (m_TypeTessellation) 
  {
    case GV::TypeTessellation::None:
    {
      float heightRaster;
      float rightHeightRaster, leftHeightRaster;
      float downHeightRaster, upHeightRaster;
      float rightDownHeightRaster, leftDownHeightRaster;
      float rightUpHeightRaster, leftUpHeightRaster;
      bool existTriangleData; 
      bool existTriangleData2; 
      bool existTriangleData3; 
      bool existTriangleData4; 
      float epsilon = 1.0e-4;
      
      std::pair<bool, bool> existActualPostVertexTriangles;

      // save if the previous 3 triangles per rectangle(previous row) row existed to known if it is necesary to save the vertex coordinates or not
      std::vector<bool> existsPreviousTriangles(width - 1, 0);
      // save if the triangles ( 3 possibles positions) on the left of a vertex exists (this vertex is part of the trinagle)
      bool existsPreviousLeftTriangle = false;
      // save if the triangles ( 3 possibles positions) on the left of a vertex in the previous row exists (this vertex is part of the trinagle)
      bool existsPreviousRowLeftTriangle = false;

      std::vector<uint32_t> nexRowCountNonDataIndices(width, 0);
      
      uint32_t totalCountNonData = 0;
      uint32_t rowCountNonData = 0;
      uint32_t totalCountNonDataIndices = 0;

      for (unsigned int i = 0; i < height - 1; i++)
      {
	rowCountNonData = 0;
	getCountNonDataRow(rowCountNonData, nexRowCountNonDataIndices, existsPreviousTriangles, epsilon, width, height, i);

	rowCountNonData += totalCountNonData; // add acumulated at the beginning of the row
	
	existsPreviousLeftTriangle = false; 
	  
	// Bucle to create the vertex data and indices data
	for (unsigned int j = 0; j < width - 1; j++)
	{
	  // Some vertex values can have a value different than NonValue but it can be isolated, not forming a triangle
	  // with this varible it can be taken itno account, adding 1 to totalCountNonData at the end of the loop
	  bool validValue = false;

	  heightRaster = m_HeightData[i * width + j];
	  rightHeightRaster = m_HeightData[i * width + j + 1];
	  downHeightRaster = m_HeightData[(i + 1) * width + j];
	  rightDownHeightRaster = m_HeightData[(i + 1) * width + j + 1];

	  bool isHeightRasterNoData = GV::Math::essentiallyEqual(heightRaster, m_NoDataValue, epsilon);
	  bool isRightHeightRasterNoData = GV::Math::essentiallyEqual(rightHeightRaster, m_NoDataValue, epsilon);
	  bool isDownHeightRasterNoData = GV::Math::essentiallyEqual(downHeightRaster, m_NoDataValue, epsilon);
	  bool isRightDownHeightRasterNoData = GV::Math::essentiallyEqual(rightDownHeightRaster, m_NoDataValue, epsilon);
	  
	  if (isHeightRasterNoData) totalCountNonData += 1; 
	  //if (a >= 2) std::cout<< heightRaster << " " << isHeightRasterNoData << " " << totalCountNonData << " " << j <<  " hei33\n";

	  // Prevent no data value knowning if there are triangles to form
	  existTriangleData = !isHeightRasterNoData && !isRightHeightRasterNoData && !isDownHeightRasterNoData;
	  existTriangleData2 = !isHeightRasterNoData && !isRightHeightRasterNoData && !isRightDownHeightRasterNoData;
	  existTriangleData3 = !isHeightRasterNoData && !isDownHeightRasterNoData && !isRightDownHeightRasterNoData;
	  existTriangleData4 = !isRightDownHeightRasterNoData && !isRightHeightRasterNoData && !isDownHeightRasterNoData;
	  
	  // correct if existTriangleData3
	  existsPreviousLeftTriangle =  existTriangleData3 ? false : existsPreviousLeftTriangle; 

	  // Previous triangles because the point can be used by previous triangles, but not a new triangle in the actual row
	  // check if the triangle on the left exists
	  if (a >= 2 && i == 2) std::cout<< existTriangleData << " " << j <<  " hei33\n";
	  if (a >= 2 && i == 2) std::cout<< existTriangleData2 << " " << j <<  " hei33\n";
	  if (a >= 2 && i == 2) std::cout<< existTriangleData3 << " " << j <<  " hei33\n";
	  if (a >= 2 && i == 2) std::cout<< existTriangleData4 << " " << j <<  " hei33\n";
	  if (a >= 2 && i == 2) std::cout<< existsPreviousTriangles[j] << " " << j <<  " PREV_TRIhei33\n";
	  if (a >= 2 && i == 2) std::cout<< existsPreviousLeftTriangle << " " << j <<  " PREV_LEFThei33\n";
	  if (a >= 2) std::cout<< rowCountNonData << " " << j <<  " ROWCOUNhei33\n";
	  if (a >= 2 && i == 2) std::cout<< nexRowCountNonDataIndices[j] << " " << j <<  " NEXhei33\n";
	  //if (a >= 2 && i == 2) std::cout<< nexRowCountNonDataIndices[j + 1] << " " << j <<  " hei33\n";

	  if (existTriangleData || existsPreviousTriangles[j] || existsPreviousLeftTriangle) 	  
	  {
	    validValue = true;
	    pushVertexValues(heightRaster, XSizePixel, YSizePixel, lengthX, lengthY, xShift, yShift, i, j);

	    //TODO completar
	    if (existTriangleData)
	    {
	      // Each row have rectangles composed of two triangles (assuming that threa are not nonData values)
	      // First triangle 
	      m_IndexData.push_back(width * i + j - totalCountNonData);
	      m_IndexData.push_back(width * i + j + 1 - totalCountNonData);
	      m_IndexData.push_back(width * (i + 1) + j - rowCountNonData - nexRowCountNonDataIndices[j]);
	    }
	    else if (existTriangleData2)
	    {
	      m_IndexData.push_back(width * i + j - totalCountNonData);
	      m_IndexData.push_back(width * i + j + 1 - totalCountNonData);
	      m_IndexData.push_back(width * (i + 1) + j + 1 - rowCountNonData - nexRowCountNonDataIndices[j]);
	    }
	  } 
	  else if (existTriangleData2)
	  {
	    validValue = true;
	    pushVertexValues(heightRaster, XSizePixel, YSizePixel, lengthX, lengthY, xShift, yShift, i, j);

	    m_IndexData.push_back(width * i + j - totalCountNonData);
	    m_IndexData.push_back(width * i + j + 1 - totalCountNonData);
	    m_IndexData.push_back(width * (i + 1) + j + 1 - rowCountNonData - nexRowCountNonDataIndices[j]);
	  }
	  else if (existTriangleData3)
	  {
	    validValue = true;
	    pushVertexValues(heightRaster, XSizePixel, YSizePixel, lengthX, lengthY, xShift, yShift, i, j);
	    
	    m_IndexData.push_back(width * i + j - totalCountNonData);
	    m_IndexData.push_back(width * (i + 1) + j - rowCountNonData - nexRowCountNonDataIndices[j]);
	    m_IndexData.push_back(width * (i + 1) + j + 1 - rowCountNonData - nexRowCountNonDataIndices[j]);
	    
	    //if (a >= 2 && i == 1 && j ==1) std::cout<< width * i + j - totalCountNonData <<" " << j <<  " hei44\n";
	    //if (a >= 2 && i == 1 && j ==1) std::cout << totalCountNonData <<" " << j <<  " hei44\n";
	  }
	  if (existTriangleData4)
	  {
	    validValue = true;
	    // Second triangle of the rectangle
	    m_IndexData.push_back(width * (i + 1) + j - rowCountNonData - nexRowCountNonDataIndices[j]);
	    m_IndexData.push_back(width * (i + 1) + j + 1 - rowCountNonData - nexRowCountNonDataIndices[j]);
	    m_IndexData.push_back(width * i + j + 1 - totalCountNonData);
	    //if (a >= 2 && i == 2) std::cout<< width*(i+1)+j-rowCountNonData-nexRowCountNonDataIndices[j] <<" " << j <<  " hhhhhei44\n";
	    //if (a >= 2 && i == 2) std::cout<< nexRowCountNonDataIndices[j] <<" " << nexRowCountNonDataIndices[j+1] <<  " hhhhhei44\n";
	  }
	  existsPreviousTriangles[j] = existTriangleData || existTriangleData3 || existTriangleData4 || existsPreviousRowLeftTriangle;
	  existsPreviousRowLeftTriangle = existTriangleData2 || existTriangleData3 || existTriangleData4;
	  existsPreviousLeftTriangle =  existTriangleData || existTriangleData2 || existTriangleData4;
	  if (!validValue && !isHeightRasterNoData) totalCountNonData += 1;
	}

	// for j = width - 1
	unsigned int j = width - 1;
	heightRaster = m_HeightData[i * width + j];
	leftHeightRaster = m_HeightData[i * width + j - 1];
	downHeightRaster = m_HeightData[(i + 1) * width + j];
	leftDownHeightRaster = m_HeightData[(i + 1) * width + j - 1];
	
	existActualPostVertexTriangles = existTrianglesActualPostVertex(heightRaster, leftHeightRaster, downHeightRaster, leftDownHeightRaster, epsilon);

	//if (existTriangleVertex(heightRaster, leftHeightRaster, downHeightRaster, leftDownHeightRaster, epsilon))
	if (existActualPostVertexTriangles.first || existsPreviousTriangles[j]) pushVertexValues(heightRaster, XSizePixel, YSizePixel, lengthX, lengthY, xShift, yShift, i, j);
	else totalCountNonData += 1;
	existsPreviousTriangles[j] = existActualPostVertexTriangles.second;
      }

      // for i = height - 1 (last row)
      existsPreviousLeftTriangle = false;
      uint32_t i = height - 1;
      for (unsigned int j = 0; j < width; j++)
      {
	bool existTrianglesVertex;
	heightRaster = m_HeightData[i * width + j];
	if (j < width - 1)
	{
	  rightHeightRaster = m_HeightData[i * width + j + 1];
	  upHeightRaster = m_HeightData[(i - 1) * width + j];
	  rightUpHeightRaster = m_HeightData[(i - 1) * width + j + 1];
	  
	  bool isHeightRasterNoData = GV::Math::essentiallyEqual(heightRaster, m_NoDataValue, epsilon);
	  bool isRightHeightRasterNoData = GV::Math::essentiallyEqual(rightHeightRaster, m_NoDataValue, epsilon);
	  bool isUpHeightRasterNoData = GV::Math::essentiallyEqual(upHeightRaster, m_NoDataValue, epsilon);
	  bool isRightUpHeightRasterNoData = GV::Math::essentiallyEqual(rightUpHeightRaster, m_NoDataValue, epsilon);
	  
	  // Prevent no data value knowning if there are triangles to form
	  existTriangleData = !isHeightRasterNoData && !isRightHeightRasterNoData && !isUpHeightRasterNoData;
	  existTriangleData2 = !isHeightRasterNoData && !isRightHeightRasterNoData && !isRightUpHeightRasterNoData;
	  existTriangleData3 = !isHeightRasterNoData && !isUpHeightRasterNoData && !isRightUpHeightRasterNoData;

	  existTrianglesVertex = existTriangleData || existTriangleData2 || existTriangleData3;
	 
	  existTriangleData4 = !isRightHeightRasterNoData && !isRightUpHeightRasterNoData && !isUpHeightRasterNoData;
	}
	else // for j = width - 1 ( the vertex in the right corner)
	{
	  leftHeightRaster = m_HeightData[i * width + j - 1];
	  upHeightRaster = m_HeightData[(i - 1) * width + j];
	  leftUpHeightRaster = m_HeightData[(i - 1) * width + j - 1];
	  existsPreviousLeftTriangle = false;

	  existTrianglesVertex = existTriangleVertex(heightRaster, leftHeightRaster, upHeightRaster, leftUpHeightRaster, epsilon);
	  existsPreviousLeftTriangle = false;
	}

	if (existTrianglesVertex || existsPreviousLeftTriangle) pushVertexValues(heightRaster, XSizePixel, YSizePixel, lengthX, lengthY, xShift, yShift, i, j);  
	else totalCountNonData += 1;
	
	existsPreviousLeftTriangle = existTriangleData || existTriangleData2 || existTriangleData4;

      } 
    }
    case GV::TypeTessellation::Uniform:
      {
	break;
      }
    case GV::TypeTessellation::NonUniform:
      break;
  }

  // Get values used to shift near the coordinate (0,0), with the first terrain introduced
  if (!shiftScaleValues.hasValues)
  {
    shiftScaleValues.xShift = m_GeoTransform[0] + lengthX/2.0;
    shiftScaleValues.yShift = m_GeoTransform[3] - lengthY/2.0;
    shiftScaleValues.xScale = (lengthX/2.0) / m_GeoTransform[0];
    shiftScaleValues.yScale = (lengthY/2.0) / m_GeoTransform[3];
    shiftScaleValues.MeanHeight = m_MeanHeight;
    shiftScaleValues.hasValues = true;
  }
    
    std::cout << "SIZE\n";
  std::cout<<m_VertexData.size()<<"   "<<m_IndexData.size()<< "\n";
    std::cout<<width<<"  "<<height<<"\n";
    std::cout<<shiftScaleValues.xShift<<"  "<<shiftScaleValues.yShift<<"\n";
    std::cout << "TERRAIN\n";
    std::cout << m_VertexData[0] <<"  ";
    std::cout << m_VertexData[1] <<"  ";
    std::cout << m_VertexData[2] <<"\n";
    std::cout << m_VertexData[m_VertexData.size() - 3] <<"  ";
    std::cout << m_VertexData[m_VertexData.size() - 2] <<"  ";
    std::cout << m_VertexData[m_VertexData.size() - 1] <<"\n";
    std::cout << " VERTEX\n\n";
  if (a >= 2){
  for (int i = 0; i < m_VertexData.size(); i+= 3)
  {
    std::cout << m_VertexData[i] <<"  ";
    std::cout << m_VertexData[i+1] <<"  ";
    std::cout << m_VertexData[i+2] <<"   INDEX: ";
    std::cout << i/3 <<"\n";
  }}
  
    std::cout << " INDICES\n\n";
  if (a >= 2){
  for (int i = 0; i < m_IndexData.size(); i+= 3)
  {
    std::cout << m_IndexData[i] <<"  ";
    std::cout << m_IndexData[i+1] <<"  ";
    std::cout << m_IndexData[i+2] <<"\n";
  }}
}

void GV::TerrainMesh::setupMesh(GV::API api, GV::BufferTypeDraw bufferTypeDraw)
{
  m_VertexArray = GV::VertexArray::create(api);
  m_VertexBuffer = GV::VertexBuffer::create(api, m_VertexData.data(), sizeof(float) * m_VertexData.size(), bufferTypeDraw);
  m_IndexBuffer = GV::IndexBuffer::create(api, m_IndexData.data(), m_IndexData.size());

  GV::BufferLayout layout = {{GV::ShaderDataType::Float3, "a_Pos"}};
  m_VertexBuffer -> setLayout(layout);

  m_VertexArray -> addVertexBuffer(m_VertexBuffer.get());
  m_VertexArray -> setIndexBuffer(m_IndexBuffer.get());
}

void GV::TerrainMesh::draw(GV::Shader& shader, const glm::mat4& viewProjectionMatrix)
{
  GV::Renderer::submit(m_VertexArray.get(), m_IndexData.size(), shader, viewProjectionMatrix);
}

void GV::TerrainMesh::pushVertexValues(float heightRaster, double XSizePixel, double YSizePixel, double lengthX, double lengthY, double xShift, double yShift, uint32_t i, uint32_t j)
{
  m_VertexData.push_back(-lengthX/2.0 + j * XSizePixel + xShift - m_MeanXCoordinate);
  m_MaxHeight = std::max(m_MaxHeight, heightRaster);
  m_MinHeight = std::min(m_MinHeight, heightRaster);
  m_VertexData.push_back(heightRaster - m_MeanHeight);
  m_VertexData.push_back(-lengthY/2.0 + i * YSizePixel + yShift - m_MeanZCoordinate);
}

bool GV::TerrainMesh::existTriangleVertex(float heightRaster, float rightLeftHeightRaster, float upDownHeightRaster, float rightLeftUpDownHeightRaster, float epsilon)
{
  bool isHeightRasterNoData = GV::Math::essentiallyEqual(heightRaster, m_NoDataValue, epsilon);
  bool isRightLeftHeightRasterNoData = GV::Math::essentiallyEqual(rightLeftHeightRaster, m_NoDataValue, epsilon);
  bool isUpDownHeightRasterNoData = GV::Math::essentiallyEqual(upDownHeightRaster, m_NoDataValue, epsilon);
  bool isRightLeftUpDownHeightRasterNoData = GV::Math::essentiallyEqual(rightLeftUpDownHeightRaster, m_NoDataValue, epsilon);

  // Triangles that can be formed with the vertex
  bool existTriangleData = !isHeightRasterNoData && !isRightLeftHeightRasterNoData && !isUpDownHeightRasterNoData;
  bool existTriangleData2 = !isHeightRasterNoData && !isRightLeftHeightRasterNoData && !isRightLeftUpDownHeightRasterNoData;
  bool existTriangleData3 = !isHeightRasterNoData && !isUpDownHeightRasterNoData && !isRightLeftUpDownHeightRasterNoData;
  return existTriangleData || existTriangleData2 || existTriangleData3;
}

std::pair<bool, bool> GV::TerrainMesh::existTrianglesActualPostVertex(float heightRaster, float rightLeftHeightRaster, float upDownHeightRaster, float rightLeftUpDownHeightRaster, float epsilon)
{
  bool isHeightRasterNoData = GV::Math::essentiallyEqual(heightRaster, m_NoDataValue, epsilon);
  bool isRightLeftHeightRasterNoData = GV::Math::essentiallyEqual(rightLeftHeightRaster, m_NoDataValue, epsilon);
  bool isUpDownHeightRasterNoData = GV::Math::essentiallyEqual(upDownHeightRaster, m_NoDataValue, epsilon);
  bool isRightLeftUpDownHeightRasterNoData = GV::Math::essentiallyEqual(rightLeftUpDownHeightRaster, m_NoDataValue, epsilon);
  
  bool existTriangleData = !isHeightRasterNoData && !isRightLeftHeightRasterNoData && !isUpDownHeightRasterNoData;
  bool existTriangleData2 = !isHeightRasterNoData && !isRightLeftHeightRasterNoData && !isRightLeftUpDownHeightRasterNoData;
  bool existTriangleData3 = !isHeightRasterNoData && !isUpDownHeightRasterNoData && !isRightLeftUpDownHeightRasterNoData;
  bool existTriangleData4 = !isUpDownHeightRasterNoData && !isRightLeftUpDownHeightRasterNoData && !isRightLeftHeightRasterNoData; 
  
  return std::make_pair(existTriangleData || existTriangleData2 || existTriangleData3, existTriangleData || existTriangleData3 || existTriangleData4);
}
GV::TerrainMesh::ExistTrianglesActualPostRightVertex GV::TerrainMesh::existTrianglesActualPostRightVertex(float heightRaster, float rightLeftHeightRaster, float upDownHeightRaster, float rightLeftUpDownHeightRaster, float epsilon)
{
  bool isHeightRasterNoData = GV::Math::essentiallyEqual(heightRaster, m_NoDataValue, epsilon);
  bool isRightLeftHeightRasterNoData = GV::Math::essentiallyEqual(rightLeftHeightRaster, m_NoDataValue, epsilon);
  bool isUpDownHeightRasterNoData = GV::Math::essentiallyEqual(upDownHeightRaster, m_NoDataValue, epsilon);
  bool isRightLeftUpDownHeightRasterNoData = GV::Math::essentiallyEqual(rightLeftUpDownHeightRaster, m_NoDataValue, epsilon);
  
  bool existTriangleData = !isHeightRasterNoData && !isRightLeftHeightRasterNoData && !isUpDownHeightRasterNoData;
  bool existTriangleData2 = !isHeightRasterNoData && !isRightLeftHeightRasterNoData && !isRightLeftUpDownHeightRasterNoData;
  bool existTriangleData3 = !isHeightRasterNoData && !isUpDownHeightRasterNoData && !isRightLeftUpDownHeightRasterNoData;
  bool existTriangleData4 = !isUpDownHeightRasterNoData && !isRightLeftUpDownHeightRasterNoData && !isRightLeftHeightRasterNoData; 
  
  return {existTriangleData || existTriangleData2 || existTriangleData3, existTriangleData || existTriangleData3 || existTriangleData4, existTriangleData || existTriangleData2 || existTriangleData4};
}

void GV::TerrainMesh::getCountNonDataRow(uint32_t& rowCountNonData, std::vector<uint32_t>& nextRowCountNonDataIndices, std::vector<bool>& existsPreviousTriangles, float epsilon, uint32_t width, uint32_t height, int i)
{
  bool existsPreviousLeftTriangles = false;
  bool nextRowExistsPreviousLeftTriangles = false;
  std::pair<bool, bool> existActualPostVertexTriangles;
  GV::TerrainMesh::ExistTrianglesActualPostRightVertex existsActualPostRightVertexTriangles;
  // check how many non values are in the actual row and the next row. Also, it is necesary to known how many values not form a triangle accounting them also as nonData
  // for k = 0
  uint32_t k = 0;
  float heightRaster = m_HeightData[i * width + k];
  float rightHeightRaster = m_HeightData[i * width + k + 1];
  float downHeightRaster = m_HeightData[(i + 1) * width + k];
  float rightDownHeightRaster = m_HeightData[(i + 1) * width + k + 1];
  
  existsActualPostRightVertexTriangles = existTrianglesActualPostRightVertex(heightRaster, rightHeightRaster, downHeightRaster, rightDownHeightRaster, epsilon); 

  if (!existsActualPostRightVertexTriangles.existsActualTriangles && !existsPreviousTriangles[k]) rowCountNonData += 1;
  
  // check next row 
  heightRaster = m_HeightData[(i + 1) * width + k];
  if (i < height - 2)
  {
    rightHeightRaster = m_HeightData[(i + 1) * width + k + 1];
    downHeightRaster = m_HeightData[(i + 2) * width + k];
    rightDownHeightRaster = m_HeightData[(i + 2) * width + k + 1];
    if(existTriangleVertex(heightRaster, rightHeightRaster, downHeightRaster, rightDownHeightRaster, epsilon) || existsActualPostRightVertexTriangles.existsPosteriorTriangles) nextRowCountNonDataIndices[0] = 0;
    else nextRowCountNonDataIndices[0] = 1;
  }
  else // last row (vertex in the left corner) 
  {
    rightHeightRaster = m_HeightData[(i + 1) * width + k + 1];
    float upHeightRaster = m_HeightData[i * width + k];
    float rightUpHeightRaster = m_HeightData[i * width + k + 1];

    if (existTriangleVertex(heightRaster, rightHeightRaster, upHeightRaster, rightUpHeightRaster, epsilon)) nextRowCountNonDataIndices[0] = 0;
    else nextRowCountNonDataIndices[0] = 1;
  }
  existsPreviousLeftTriangles = existsActualPostRightVertexTriangles.existsRightTriangles;

  for (unsigned int k = 1; k < width - 1; k++)
  {
    heightRaster = m_HeightData[i * width + k];
    rightHeightRaster = m_HeightData[i * width + k + 1];
    downHeightRaster = m_HeightData[(i + 1) * width + k];
    rightDownHeightRaster = m_HeightData[(i + 1) * width + k + 1];
   
    existsActualPostRightVertexTriangles = existTrianglesActualPostRightVertex(heightRaster, rightHeightRaster, downHeightRaster, rightDownHeightRaster, epsilon); 
    
    if (!existsActualPostRightVertexTriangles.existsActualTriangles && !existsPreviousTriangles[k] && !existsPreviousLeftTriangles) rowCountNonData += 1;
    //if (i == 2) std::cout<< rowCountNonData<< "  " << existsPreviousTriangles[k]<<"  "<< existsPreviousLeftTriangles << " " << k<<   " HHHHhei33\n";
    existsPreviousLeftTriangles = existsActualPostRightVertexTriangles.existsRightTriangles;

    // check next row 
    heightRaster = m_HeightData[(i + 1) * width + k];
    if (i < height - 2)
    {
      rightHeightRaster = m_HeightData[(i+ 1) * width + k + 1];
      downHeightRaster = m_HeightData[(i + 2) * width + k];
      rightDownHeightRaster = m_HeightData[(i + 2) * width + k + 1];
      
      if (existTriangleVertex(heightRaster, rightHeightRaster, downHeightRaster, rightDownHeightRaster, epsilon) || existsActualPostRightVertexTriangles.existsPosteriorTriangles) nextRowCountNonDataIndices[k] = nextRowCountNonDataIndices[k - 1];
      else nextRowCountNonDataIndices[k] = nextRowCountNonDataIndices[k - 1] + 1;
    }
    else  // last row
    {
      rightHeightRaster = m_HeightData[(i + 1) * width + k + 1];
      float upHeightRaster = m_HeightData[i * width + k];
      float rightUpHeightRaster = m_HeightData[i * width + k + 1]; 

      existsActualPostRightVertexTriangles = existTrianglesActualPostRightVertex(heightRaster, rightHeightRaster, upHeightRaster, rightUpHeightRaster, epsilon); 

      if (existsActualPostRightVertexTriangles.existsActualTriangles || nextRowExistsPreviousLeftTriangles) nextRowCountNonDataIndices[k] = nextRowCountNonDataIndices[k - 1];
      else nextRowCountNonDataIndices[k] = nextRowCountNonDataIndices[k - 1] + 1;
      
      nextRowExistsPreviousLeftTriangles = existsActualPostRightVertexTriangles.existsRightTriangles;
    }
  }

  // for k = width - 1
  k = width - 1;
  heightRaster = m_HeightData[i * width + k];
  float leftHeightRaster = m_HeightData[i * width + k - 1];
  downHeightRaster = m_HeightData[(i + 1) * width + k];
  float leftDownHeightRaster = m_HeightData[(i + 1) * width + k - 1];
  
  existActualPostVertexTriangles = existTrianglesActualPostVertex(heightRaster, leftHeightRaster, downHeightRaster, leftDownHeightRaster, epsilon);

  if (!existActualPostVertexTriangles.first && !existsPreviousTriangles[k]) rowCountNonData += 1;
  
  // check next row
  heightRaster = m_HeightData[(i + 1) * width + k];
  if (i < height - 2)
  {
    leftHeightRaster = m_HeightData[(i + 1) * width + k - 1];
    downHeightRaster = m_HeightData[(i + 2) * width + k];
    leftDownHeightRaster = m_HeightData[(i + 2) * width + k - 1];
    if (existTriangleVertex(heightRaster, leftHeightRaster, downHeightRaster, leftDownHeightRaster, epsilon) || existActualPostVertexTriangles.second) nextRowCountNonDataIndices[k] = nextRowCountNonDataIndices[k - 1];
    else nextRowCountNonDataIndices[k] = nextRowCountNonDataIndices[k - 1] + 1;
  }
  else  // last row (vertex in the right corner)
  {
    leftHeightRaster = m_HeightData[(i + 1) * width + k - 1];
    float upHeightRaster = m_HeightData[i * width + k];
    float leftUpHeightRaster = m_HeightData[i * width + k - 1];

    if (!existTriangleVertex(heightRaster, leftHeightRaster, upHeightRaster, leftUpHeightRaster, epsilon)) nextRowCountNonDataIndices[k] = nextRowCountNonDataIndices[k - 1] + 1;
    else nextRowCountNonDataIndices[k] = nextRowCountNonDataIndices[k - 1];
  }
  if (i == 3) std::cout<< rowCountNonData<< "  " << k<<   " HHHHhei33\n";
}
