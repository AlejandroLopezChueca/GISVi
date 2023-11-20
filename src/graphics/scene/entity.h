#pragma once

#include "ui/panels/logPanel.h"
//#include "components.h"
#include "graphics/UUID.h"
#include "scene.h"

#include "entt.hpp"
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace GV
{
  class Entity
  {
    public:
      Entity() = default;
      Entity(entt::entity entityHandle, GV::Scene* scene);

      template<typename T, typename... Args>
      T& addComponent(Args&&... args)
      {
	if (hasComponent<T>())
	{
	  std::string message = "Entity already has component";
	  ImGuiLogPanel::addLog(message, LogCategory::Warning);
	}
	T& component = m_Scene -> m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
	//m_Scene -> onComponentAdded(*this, component);
	return component;
      }

      template<typename T>
      T& getComponent() const
      {
	if (!hasComponent<T>())
	{
	  std::string message = "Entity does not hav component";
	  ImGuiLogPanel::addLog(message, LogCategory::Error);
	}
	return m_Scene -> m_Registry.get<T>(m_EntityHandle);
      }

      template<typename T>
      void removeComponent()
      {
	if (!hasComponent<T>())
	{
	  std::string message = "Entity does not hav component";
	  ImGuiLogPanel::addLog(message, LogCategory::Error);
	}
	else m_Scene -> m_Registry.remove<T>(m_EntityHandle);
      }

      template<typename T>
      bool hasComponent() const
      { 
	return m_Scene -> m_Registry.all_of<T>(m_EntityHandle);
      }

      GV::UUID getUUID();
      std::string& getName();
      bool& getDrawAction();
      bool hasChilds();
      bool isParent();
      std::vector<Entity>& getChildsEntities();
      uint32_t& getChildId();

      operator bool() const {return m_EntityHandle != entt::null;}
      operator entt::entity() const {return m_EntityHandle;}
      operator uint32_t() const {return (uint32_t)m_EntityHandle;}

      bool operator ==(const Entity& other) const
      {
	return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
      }

      bool operator !=(const Entity& other) const
      {
	return !(*this == other);
      }

      bool draw = true;

    private:
      entt::entity m_EntityHandle{entt::null};

      GV::Scene* m_Scene = nullptr;


  };

}
