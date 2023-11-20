
#include "hierarchyPanel.h"
#include "graphics/API.h"
#include "graphics/scene/components.h"
#include "graphics/scene/entity.h"
#include "graphics/scene/scene.h"
#include "graphics/texture.h"
#include "ui/uiUtils.h"
#include "graphics/renderer.h"
#include "graphics/colorMaps.h"

#include "imgui.h"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include "glm/gtc/type_ptr.hpp"

std::array<std::unique_ptr<GV::Texture2D>, 4> GV::HierarchyPanel::s_ColorMaps;

GV::HierarchyPanel::HierarchyPanel(GV::API api)
{
  createTextures(api);
}

GV::HierarchyPanel::HierarchyPanel(GV::Scene* scene, GV::API api)
{
  setContext(scene);
  createTextures(api);
}

void GV::HierarchyPanel::setContext(GV::Scene* scene)
{
  m_Context = scene;
  m_SelectedEntity = {};
}

void GV::HierarchyPanel::createTextures(GV::API api)
{
  s_ColorMaps[0] = GV::Texture2D::create(api, "resources/icons/colorMaps/gray.png");
  s_ColorMaps[1] = GV::Texture2D::create(api, "resources/icons/colorMaps/imola.png");
  s_ColorMaps[2] = GV::Texture2D::create(api, "resources/icons/colorMaps/roma.png");
  s_ColorMaps[3] = GV::Texture2D::create(api, "resources/icons/colorMaps/devon.png");
}

void GV::HierarchyPanel::onImGuiRender()
{
  ImGui::Begin("Layers");
  if (m_Context)
  {
    //for (auto entityID : m_Context -> m_Registry.storage<entt::entity>())
    for (auto entityID : m_Context -> m_Registry.view<GV::ParentComponent>())
    {
      GV::Entity entity{entityID, m_Context};
      drawParentEntityNode(entity);
    };

    // left click. Deselect entity
    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
    {
      m_SelectedEntity = {};
      m_SelectedChildID = 0;
    }
  }
  ImGui::End();

  ImGui::Begin("Properties");
  if (m_SelectedEntity) drawComponents();
  ImGui::End();
}

void GV::HierarchyPanel::setSelectedEntity(GV::Entity& entity)
{
  m_SelectedEntity = entity;
}

void GV::HierarchyPanel::drawParentEntityNode(GV::Entity& entity)
{
  const std::string& tag = entity.getName();
  bool& draw = entity.getDrawAction();
  ImGui::SetNextItemAllowOverlap();
  ImGui::PushID((int)(uint32_t)entity);
  ImGui::Checkbox("##", &draw);
  ImGui::PopID();
  ImGui::SameLine();

  ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
  flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
  bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

  if (ImGui::IsItemClicked()) 
  {
    m_SelectedEntity = entity;
    if (entity.hasChilds())
    {
      uint32_t& childID = entity.getChildId();
      m_SelectedChildID = 1; // parent
      childID = 1;
    }
  }

  bool entityDeleted = false;
  if (ImGui::BeginPopupContextItem())
  {
    if (ImGui::MenuItem("Zoom to Entity"))
      if(m_SelectedEntity) m_CameraController -> zoomToEntity(m_SelectedEntity);
    ImGui::Separator();
    if (ImGui::MenuItem("Delete Entity")) entityDeleted = true;
    ImGui::EndPopup();
  }

  if (opened) 
  {
    if (entity.hasChilds())
    {
      uint32_t& childID = entity.getChildId();
      // 0 means to do nothing and 1 to modify all childs
      int idx = 2; 
      for (auto childEntity : entity.getChildsEntities()) 
      {
	drawChildEntityNode(childEntity, idx);
	idx += 1 ;
      }
      childID = m_SelectedChildID;
    }
    ImGui::TreePop();
  }

  if (entityDeleted)
  {
    m_Context->destroyEntity(entity);
    if (m_SelectedEntity == entity) m_SelectedEntity = {};
  }
}

void GV::HierarchyPanel::drawChildEntityNode(GV::Entity& entity, int idx)
{
  const std::string& tag = entity.getName();
  
  ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen;

  ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
  
  if (ImGui::IsItemClicked()) 
  {
    m_SelectedEntity = entity;
    m_SelectedChildID = idx;
  }
  
  if (ImGui::BeginPopupContextItem())
  {
    if (ImGui::MenuItem("Zoom to Entity"))
      if(m_SelectedEntity) m_CameraController -> zoomToEntity(m_SelectedEntity);
    ImGui::EndPopup();
  }
}

void GV::HierarchyPanel::drawComponents()
{
  if (m_SelectedEntity.hasComponent<GV::TagComponent>())
  {
    std::string& tag = m_SelectedEntity.getName();
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, tag.c_str(), sizeof(buffer));
    if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) tag = std::string(buffer);
  }

  GV::UiUtils::drawComponent<GV::TransformComponent>("Transform", m_SelectedEntity, [](auto& component)
  {
    GV::UiUtils::drawVec3Control("Translation", component.translation, component.originalTranslationValues);
    glm::vec3 rotation = glm::degrees(component.rotation); // convert radians to degrees
    GV::UiUtils::drawVec3Control("Rotation", rotation);
    component.rotation = glm::radians(rotation); // degress to radians
    GV::UiUtils::drawVec3Control("Scale", component.scale, {1.0f, 1.0f, 1.0f});
  });

  GV::UiUtils::drawComponent<GV::CubeMeshComponent>("Cube Renderer", m_SelectedEntity, [](auto& component)
  {
    ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
  });

  GV::UiUtils::drawComponent<GV::TerrainMeshComponent>("Terrain Renderer", m_SelectedEntity, [](auto& component)
  {
    ImGui::Text("Gradients");
    ImGui::DragFloatRange2("##", &component.maxMinMeanHeight.y, &component.maxMinMeanHeight.x, 0.25f, 0.0f, 0.0f, "Min: %.2f", "Max: %.2f");
      
    int& currentIdxColormap = component.currentIdxColormap;
    GV::UiUtils::drawImageComboBox<4>(GV::HierarchyPanel::getColorMaps(), GV::ColorMaps::nameColorMaps, currentIdxColormap);
  });
  
  GV::UiUtils::drawComponent<GV::ClippingPlaneComponent>("Clipping Plane Renderer", m_SelectedEntity, [](auto& component)
  {
    ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
  });

  GV::UiUtils::drawComponent<GV::SummaryDatasetHECRAS>("Summary Dataset HEC-RAS", m_SelectedEntity, [](auto& component)
  {
    ImGui::Text("Gradients");
    ImGui::DragFloatRange2("##", &component.maxMinMeanValues.y, &component.maxMinMeanValues.x, 0.25f, 0.0f, 0.0f, "Min: %.2f", "Max: %.2f");
    
    int& currentIdxColormap = component.currentIdxColormap;
    GV::UiUtils::drawImageComboBox<4>(GV::HierarchyPanel::getColorMaps(), GV::ColorMaps::nameColorMaps, currentIdxColormap); 
  });

}
