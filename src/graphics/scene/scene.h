#pragma once

#include "graphics/UUID.h"
#include "graphics/camera.h"

#include "entt.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>



namespace GV 
{
  struct ShiftScaleValues
  {
    double xShift = 0.0;
    double yShift = 0.0;
    double xScale = 1.0;
    double yScale = 1.0;
    double MeanHeight = 0.0;
    bool hasValues = false;
  };

  class Entity;

  class Scene
  {
    public:
      Scene();
      ~Scene();

      GV::Entity createEntity(const std::string& name = std::string(), bool isParent = true);
      GV::Entity createEntityWithUUID(GV::UUID uuid, const std::string& name = std::string(), bool isParent = true);

      void destroyEntity(GV::Entity& entity);

      void update(const GV::PerspectiveCamera& camera);

      /*template<typename T>
      void onComponentAdded(GV::Entity entity, T& component);*/

      void resizeViewport(uint32_t width, uint32_t height);

      // Due to the large values that coordinates normally have in reference systems. All coordinates will be shifted to be near (0,0). The values used will correspond with the first terrain loaded
      static bool s_ShiftScalehasValues;

      ShiftScaleValues m_ShiftScaleValues;

    private:
      entt::registry m_Registry;

      uint32_t m_ViewportWidth = 0;
      uint32_t m_ViewportHeight = 0;

      uint32_t m_CountClippingPlane = 0;

      std::unordered_map<GV::UUID, entt::entity> m_EntityMap;
      //std::unordered_map<entt::entity, bool> m_EntityDrawMap;


      friend class Entity;
      friend class HierarchyPanel;
      friend class CubesFromPointsPanel;
      friend class EditorLayer;
  };

}
