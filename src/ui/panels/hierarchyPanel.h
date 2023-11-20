#pragma once

#include "graphics/API.h"
#include "graphics/cameraController.h"
#include "graphics/scene/entity.h"
#include "graphics/scene/scene.h"
#include "graphics/texture.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <array>

namespace GV
{
  class HierarchyPanel
  {
    public:
      HierarchyPanel() = default;
      HierarchyPanel(GV::API api);
      HierarchyPanel(GV::Scene* scene, GV::API api);

      void setContext(GV::Scene* scene);
      void setCameraController(GV::PerspectiveCameraController* cameraController) {m_CameraController = cameraController;}
      void createTextures(GV::API api);


      void onImGuiRender();

      GV::Entity& getSelectedEntity() {return m_SelectedEntity;}
      void setSelectedEntity(GV::Entity& entity);

      static std::array<std::unique_ptr<GV::Texture2D>, 4>& getColorMaps() {return s_ColorMaps;}


    private:
      void drawParentEntityNode(GV::Entity& entity);
      void drawChildEntityNode(GV::Entity& entity, int idx);
      void drawComponents();
      
      GV::Scene* m_Context;
      GV::Entity m_SelectedEntity;
      GV::PerspectiveCameraController* m_CameraController;
      uint32_t m_SelectedChildID = 0;

      static std::array<std::unique_ptr<GV::Texture2D>, 4> s_ColorMaps;

      enum class Type
      {
	Asset = 0, FileSystem = 1
      };

      Type m_Type = Type::Asset;

  };
}
