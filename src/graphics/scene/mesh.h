#pragma once

#include "graphics/API.h"
#include "graphics/buffer.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/vertexArray.h"
#include "io/reader.h"
#include "scene.h"

#include "glm/fwd.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>
#include <tuple>
#include <vector>

namespace GV
{
  enum class MeshTypeCreation
  {
    Batching = 0, Instancing
  };

  enum class TypeInstancing
  {
    MonoEntity = 0, MultiEntity
  };

  class Mesh
  {
    public:
      Mesh() = default;
      //Mesh(GV::API api, const std::string& path, MeshTypeCreation meshTypeCreation, GV::BufferTypeDraw bufferTypeDraw);
 
      float getMeanHeight() const {return m_MeanHeight;}
      float getMeanXCoordinate() const {return m_MeanXCoordinate;}
      float getMeanZCoordinate() const {return m_MeanZCoordinate;}

      float getXValueShift() const {return m_XValueShift;}
      float getHeightValueShift() const {return m_HeightValueShift;}
      float getZValueShift() const {return m_ZValueShift;}

      std::vector<glm::vec3>& getTranslationData() {return m_TranslationData;}
      
      //void getIndexData();
      size_t getIndexCount() const {return m_IndexData.size();}
      uint32_t getInstanceCount() const {return m_InstanceCount;}
      GV::VertexArray* getVertexArray() const {return m_VertexArray.get();}

    protected:

      std::vector<glm::mat4> calculateTransformInstanceData();

      std::vector<float> m_VertexData;
      std::vector<unsigned int> m_IndexData;
      std::vector<float> m_OffsetData;
      std::vector<glm::vec3> m_TranslationData;
      //std::vector<glm::mat4> m_TransformData;

      std::unique_ptr<GV::VertexArray> m_VertexArray;
      std::unique_ptr<GV::VertexBuffer> m_VertexBuffer;
      std::unique_ptr<GV::VertexBuffer> m_OffsetBuffer;
      std::unique_ptr<GV::VertexBuffer> m_TransformBuffer;
      std::unique_ptr<GV::IndexBuffer> m_IndexBuffer;
      std::unique_ptr<GV::Texture2D> m_Texture;

      float m_MeanHeight = 0.0f;
      float m_MeanXCoordinate = 0.0f;
      float m_MeanZCoordinate = 0.0f;
      float m_XValueShift = 0.0f;
      float m_HeightValueShift = 0.0f;
      float m_ZValueShift = 0.0f;
      
      uint32_t m_InstanceCount = 0;
  };

  struct CubeMeshParameters
  {
    GV::API api;
    uint32_t rasterWidth;
    float rasterMeanHeight;
    float xWidthCube = 0.0f;
    float zWidthCube = 0.0f;
    float heightCube = 0.0f;
    MeshTypeCreation meshTypeCreation;
    GV::BufferTypeDraw bufferTypeDraw;
    TypeInstancing typeInstancing;
  };

  class CubeMesh: public Mesh
  {
    public:
      CubeMesh(const std::string& pathData, const std::vector<float>& rasterHeightData, const std::array<double, 6>& rasterGeoTransform, GV::ShiftScaleValues& shiftScaleValues, const CubeMeshParameters& parameters, const bool setMesh);

    private:
      void getVertexDataFromPoints(const std::string& pathPoints, const std::vector<float>& rasterHeightData, const std::array<double, 6>& rasterGeoTransform, GV::ShiftScaleValues& shiftScaleValues, const CubeMeshParameters& parameters);

      void batchCubesFromPoints(std::vector<float>& pointsCoordinates, const CubeMeshParameters& CubeMeshParameters);
      void instanceCubesFromPoints(std::vector<float>& pointsCoordinates,const CubeMeshParameters& parameters, const GV::ShiftScaleValues& shiftScaleValues, const GV::VectorReader::MeanValuesCoordinates& meanValuesCoordinates);
      void setupMesh(const GV::CubeMeshParameters& parameters);
       
      float m_XWidthCube, m_ZWidthCube, m_HeightCube;
  };

  enum class TypeTessellation
  {
    None = 0, Uniform, NonUniform
  };

  class TerrainMesh
  {
    public:
      TerrainMesh(GV::API api, TypeTessellation typeTessellation, const std::string& path, GV::ShiftScaleValues& shiftScaleValues, GV::BufferTypeDraw bufferTypeDraw);

      void getVertexData(std::string path, GV::ShiftScaleValues& shiftScaleValues);
      void setupMesh(GV::API api, GV::BufferTypeDraw bufferTypeDraw);
      void draw(GV::Shader& shader, const glm::mat4& viewProjectionMatrix);
      float getMaxValue() const {return m_MaxHeight;}
      float getMinValue() const {return m_MinHeight;}
      float getMeanHeight() const {return m_MeanHeight;}
      float getMeanXCoordinate() const {return m_MeanXCoordinate;}
      float getMeanZCoordinate() const {return m_MeanZCoordinate;}

      uint32_t getRasterWidth() const {return m_RasterWidth;}

      const std::vector<float>& getHeightData() const {return m_HeightData;}
      const std::array<double, 6>& getGeotransform() const {return m_GeoTransform;}

      size_t getIndexCount() const {return m_IndexData.size();}
      const GV::VertexArray* getVertexArray() const {return m_VertexArray.get();}
      
     private:

      void pushVertexValues(float heightRaster, double XSizePixel, double YSizePixel, double lengthX, double lengthY, double xShift, double yShift, uint32_t i, uint32_t j);

      /**
       * @brief Check the existence of the triangles that a vertex data (heightRaster) can be part
       *
       *
       */
      bool existTriangleVertex(float heightRaster, float rightLeftHeightRaster, float upDownHeightRaster, float rightLeftUpDownHeightRaster, float epsilon);

      /**
       *
       *
       */
      std::pair<bool, bool> existTrianglesActualPostVertex(float heightRaster, float rightLeftHeightRaster, float upDownHeightRaster, float rightLeftUpDownHeightRaster, float epsilon);
 
      struct ExistTrianglesActualPostRightVertex
      {
	bool existsActualTriangles;
	bool existsPosteriorTriangles;
	bool existsRightTriangles;
      };
      /**
       * @ brief Check the existence of the triangles than a vertex, the same vertex in the next row and the adjacent vertex on the left can be part
       *
       */
      ExistTrianglesActualPostRightVertex existTrianglesActualPostRightVertex(float heightRaster, float rightLeftHeightRaster, float upDownHeightRaster, float rightLeftUpDownHeightRaster, float epsilon);
      
      /**
       * @brief Get how many non values are in the actual row and the next row. Also, it is necesary to known how many values not form a triangle accounting them also as nonData 
       *
       */
      void getCountNonDataRow(uint32_t& rowCountNonData, std::vector<uint32_t>& nextRowCountNonDataIndices, std::vector<bool>& existsPreviousTriangles, float epsilon, uint32_t width, uint32_t height, int i);
      //void checkExistenceNonValidData(uint32_t& totalCountNonData, uint32_t& rowCountNonData);

      std::vector<float> m_HeightData;
      std::vector<float> m_VertexData;
      std::vector<uint32_t> m_IndexData;

      std::unique_ptr<GV::VertexArray> m_VertexArray;
      std::unique_ptr<GV::VertexBuffer> m_VertexBuffer;
      std::unique_ptr<GV::IndexBuffer> m_IndexBuffer;
      std::unique_ptr<GV::Texture2D> m_Texture;

      TypeTessellation m_TypeTessellation;
      std::string m_Path;

      float m_NoDataValue = 0.0f;

      std::unique_ptr<GV::Texture2D> ortophotoTexture;

      //A geotransform is an affine transformation from the image coordinate space (row, column), also known as (pixel, line) to the georeferenced coordinate space (projected or geographic coordinates).
      //
      // GT(0) x-coordinate of the upper-left corner of the upper-left pixel.
      //GT(1) w-e pixel resolution / pixel width.
      //GT(2) row rotation (typically zero).
      //GT(3) y-coordinate of the upper-left corner of the upper-left pixel.
      //GT(4) column rotation (typically zero).
      //GT(5) n-s pixel resolution / pixel height (negative value for a north-up image).
      std::array<double, 6> m_GeoTransform;

      float m_MaxHeight = 0;
      float m_MinHeight = 0;
      float m_MeanHeight = 0;
      float m_MeanXCoordinate = 0;
      float m_MeanZCoordinate = 0;

      uint32_t m_RasterWidth = 0;
  };

}
