#include "entity.h"
#include "components.h"
#include <cstdint>

GV::Entity::Entity(entt::entity handle, GV::Scene* scene)
   : m_EntityHandle(handle), m_Scene(scene)
{
}


GV::UUID GV::Entity::getUUID()
{
  return getComponent<GV::IDComponent>().ID;
}

std::string& GV::Entity::getName()
{
  return getComponent<GV::TagComponent>().tag;
}
bool& GV::Entity::getDrawAction()
{
  return getComponent<GV::DrawComponent>().draw;
}

bool GV::Entity::hasChilds()
{
  
  return getComponent<GV::ParentComponent>().hasChilds;
}

bool GV::Entity::isParent()
{
  return hasComponent<GV::ParentComponent>();  
}

std::vector<GV::Entity>& GV::Entity::getChildsEntities()
{
  return getComponent<GV::ChildEntitiesComponent>().childEntities;
}

uint32_t& GV::Entity::getChildId()
{
  return getComponent<GV::TransformComponent>().childID;
}

