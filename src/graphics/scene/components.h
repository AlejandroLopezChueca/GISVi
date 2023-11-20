#pragma once

#include "graphics/UUID.h"
#include "graphics/buffer.h"
#include "graphics/scene/mesh.h"
#include "graphics/API.h"
#include "graphics/scene/scene.h"
#include "graphics/scene/entity.h"
#include "graphics/texture.h"
#include "graphics/vertexArray.h"


#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/quaternion.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace GV
{
  struct DrawComponent
  {
    bool draw = true;

    DrawComponent() = default;
    DrawComponent(const DrawComponent&) = default;
  };

  struct IDComponent
  {
    GV::UUID ID;

    IDComponent() = default;
    IDComponent(const IDComponent&) = default;
  };

  struct TagComponent
  {
    std::string tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& str)
      : tag(str) {}
  };
  
  // Empty struct to classify an entity as a parent of others
  struct ParentComponent
  {
    // Prevent error in creation component
    bool hasChilds = false;

    ParentComponent() = default;
    ParentComponent(const bool hasChildsEntity)
      :hasChilds(hasChildsEntity)
    {}
  };


  struct ChildEntitiesComponent
  {
    std::vector<GV::Entity> childEntities;
    //std::vector<glm::vec3*> childsTranslation;
    GV::Scene* context;

    ChildEntitiesComponent() = default;
    ChildEntitiesComponent(const ChildEntitiesComponent&) = default;
    ChildEntitiesComponent(GV::Scene* scene)
      :context(scene) {}
    //TODO delete entitites
    ~ChildEntitiesComponent()
    {
      for (auto entity : childEntities)
      {
	// TODO Generate some error
	context -> destroyEntity(entity);
      }
    }
  };

  struct TransformComponent
  {
    glm::vec3 translation = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    
    glm::vec3 originalTranslationValues = {0.0f, 0.0f, 0.0f};
    
    // Index of the child when the mesh is a instancing type. It is used to select it's tranform in the data of the parent
    uint32_t childID = 0;

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& translationValues)
      : translation(translationValues), originalTranslationValues(translationValues) {}

    glm::mat4 getTransform() const
    {
      glm::mat4 identity = glm::mat4(1.0f);
      glm::mat4 rotationMatrix = glm::toMat4(glm::quat(rotation));
      return glm::translate(identity, translation) * rotationMatrix * glm::scale(identity, scale);
    }
  };

  struct CubeMeshComponent
  {
    glm::vec4 color{1.0f, 0.0f, 0.0f, 1.0f};
    std::unique_ptr<GV::Mesh> mesh;
    size_t indexCount;
    GV::VertexArray* vertexArray;
    GV::MeshTypeCreation meshTypeCreation;
    GV::TypeInstancing typeInstancing;
    uint32_t instanceCount;
    bool* draw;

    //CubeMeshComponent() = default;
    //CubeMeshComponent(const CubeMeshComponent&) = default;
    CubeMeshComponent(const std::string& pathData, const std::vector<float>& rasterHeightData, const std::array<double, 6>& rasterGeoTransform, GV::ShiftScaleValues& shiftScaleValues, const GV::CubeMeshParameters& parameters, bool* drawComponent, const bool setMesh = true) 
      : draw(drawComponent), meshTypeCreation(parameters.meshTypeCreation), typeInstancing(parameters.typeInstancing)
    {
      mesh = std::make_unique<GV::CubeMesh>(pathData, rasterHeightData, rasterGeoTransform, shiftScaleValues, parameters, setMesh);
      indexCount = mesh -> getIndexCount();
      vertexArray = mesh -> getVertexArray();
      instanceCount = mesh -> getInstanceCount();
    }
  };

  struct TerrainMeshComponent
  {
    std::unique_ptr<GV::TerrainMesh> mesh;
    size_t indexCount;
    glm::vec3 maxMinMeanHeight;

    int currentIdxColormap = 0;

    const GV::VertexArray* vertexArray;
    bool* draw;

    //TerrainMeshComponent() = default;
    //TerrainMeshComponent(const TerrainMeshComponent&) = default;
    TerrainMeshComponent(GV::API api, GV::TypeTessellation typeTessellation, const std::string& path, GV::ShiftScaleValues& shiftScaleValues, GV::BufferTypeDraw bufferTypeDraw, bool* drawComponent) 
      : draw(drawComponent)
    {
      mesh = std::make_unique<GV::TerrainMesh>(api, typeTessellation, path, shiftScaleValues, bufferTypeDraw);
      indexCount = mesh -> getIndexCount();
      vertexArray = mesh -> getVertexArray();
      maxMinMeanHeight = {mesh -> getMaxValue(), mesh -> getMinValue(), mesh -> getMeanHeight()};
    }

  };

  struct ClippingPlaneComponent
  {
    glm::vec4 color{1.0f, 0.0f, 0.0f, 1.0f};
    glm::vec3 planeNormal = {1.0f, 0.0f, 0.0f};
    glm::vec3 PointOnPlane  = {0.0f, 0.0f, 0.0f};
    bool* draw;

    ClippingPlaneComponent(bool* drawComponent)
      : draw(drawComponent) 
    {

    }

  };

  struct SummaryDatasetHECRAS
  {
    //std::unique_ptr<GV::TerrainMesh> mesh;
    glm::vec3 maxMinMeanValues;

    int currentIdxColormap = 0;
    
    bool* draw;
    
    const GV::VertexArray* vertexArray;

    SummaryDatasetHECRAS(GV::API api, GV::TypeTessellation typeTessellation, bool* drawComponent)
      : draw(drawComponent)
    {

    }

  };

  struct TimeSeriesDatasetHECRAS
  {
    float time;
    float duration;
    
    bool* draw;

    TimeSeriesDatasetHECRAS(bool* drawComponent)
      : draw(drawComponent)
    {

    }

  };

  struct DirectionalLight
  {
    // angle of the light with respect the 3 axis
    //    y
    //    |
    //    |
    //    |______ x
    //   /
    //  / 
    // z 
    glm::vec3 angle{0.0f, 0.0f, 0.0f};
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f}; //white

  };

}
