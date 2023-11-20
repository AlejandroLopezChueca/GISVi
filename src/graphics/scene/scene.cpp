
#include "scene.h"

#include "components.h"
#include "graphics/camera.h"
#include "graphics/renderer.h"
#include "graphics/UUID.h"
#include "entity.h"
#include "mesh.h"

#include <cstdint>
#include <iostream>





//////////////////////////////////////// Scene ////////////////////////////////////////

GV::Scene::Scene()
{
}

GV::Scene::~Scene()
{

}

GV::Entity GV::Scene::createEntity(const std::string& name, bool isParent)
{
  return createEntityWithUUID(GV::UUID(), name, isParent);
}

GV::Entity GV::Scene::createEntityWithUUID(GV::UUID uuid, const std::string& name, bool isParent)
{
  GV::Entity entity = GV::Entity(m_Registry.create(), this);
  entity.addComponent<GV::IDComponent>(uuid);
  entity.addComponent<GV::TransformComponent>();
  if (isParent) entity.addComponent<GV::ParentComponent>();
  auto& tag = entity.addComponent<GV::TagComponent>();
  tag.tag = name.empty() ? "Entity" : name;
  entity.addComponent<GV::DrawComponent>();

  m_EntityMap[uuid] = entity;

  return entity;
}

void GV::Scene::destroyEntity(GV::Entity& entity)
{
  m_EntityMap.erase(entity.getUUID());
  m_Registry.destroy(entity);
}

void GV::Scene::update(const GV::PerspectiveCamera& camera)
{
  // Draw clipping planes
  if (m_CountClippingPlane)
  {
    auto group = m_Registry.group<GV::ClippingPlaneComponent>(entt::get<GV::TransformComponent>);
    int idx = 0;
    for (auto entity : group)
    {
      auto [clippingPlaneComponent, transformComponent] = group.get<GV::ClippingPlaneComponent, GV::TransformComponent>(entity);

      //std::cout << clippingPlaneComponent.planeNormal[0] << "  " << clippingPlaneComponent.planeNormal[1] << " " <<clippingPlaneComponent.planeNormal[2] << "\n\n";
      //std::cout << transformComponent.rotation[0] << "  " << transformComponent.rotation[1] << " " <<transformComponent.rotation[2] << "\n";
      //clippingPlaneComponent.planeNormal = glm::toMat3(glm::quat(transformComponent.rotation)) * clippingPlaneComponent.planeNormal;
      clippingPlaneComponent.PointOnPlane = transformComponent.translation;
      if(*clippingPlaneComponent.draw) GV::Renderer::drawClippingPlane(clippingPlaneComponent.planeNormal, clippingPlaneComponent.PointOnPlane, transformComponent.rotation, idx);
      idx += 1;
    }
    GV::Renderer::sendDataClippingPlanes();
  }
  // Draw cubes
  {
    auto view = m_Registry.view<GV::TransformComponent, GV::CubeMeshComponent>();
    //if (view.size_hint() > 0) GV::Renderer::bindCubeShader();
    for (auto entity : view)
    {
      auto [transformComponent, cubeMeshComponent] = view.get<GV::TransformComponent, GV::CubeMeshComponent>(entity);

      switch (cubeMeshComponent.meshTypeCreation) 
      { 
	case GV::MeshTypeCreation::Batching:
	{
	  if (*cubeMeshComponent.draw) GV::Renderer::drawCube(cubeMeshComponent.vertexArray, cubeMeshComponent.indexCount, transformComponent.getTransform(), cubeMeshComponent.color);
	  break;
	}
	case GV::MeshTypeCreation::Instancing:
	{
	  if (*cubeMeshComponent.draw) GV::Renderer::drawInstancedCube(transformComponent, cubeMeshComponent, entity);
	  break;
	}
      };
    }
  }
 
  // Draw Terrain
  {
    auto group = m_Registry.group<GV::TerrainMeshComponent>(entt::get<GV::TransformComponent>);

    if (group.size() > 0) GV::Renderer::bindTerrainShader(); 

    for (auto entity : group)
    {
      auto [terrainMeshComponent, transformComponent] = group.get<GV::TerrainMeshComponent, GV::TransformComponent>(entity);

      if (*terrainMeshComponent.draw) GV::Renderer::drawTerrain(terrainMeshComponent.vertexArray, terrainMeshComponent.indexCount, transformComponent.getTransform(), camera.getViewProjectionMatrix(), terrainMeshComponent.maxMinMeanHeight, terrainMeshComponent.currentIdxColormap);
    }
  }
}

/*template<typename T>
void GV::Scene::onComponentAdded(GV::Entity entity, T &component)
{

}*/

void GV::Scene::resizeViewport(uint32_t width, uint32_t height)
{
  if (m_ViewportWidth == width && m_ViewportHeight == height) return;

  m_ViewportWidth = width;
  m_ViewportHeight = height;

}

