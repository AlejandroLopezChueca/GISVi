#pragma once

#include "glm/ext/vector_float3.hpp"
#include "graphics/scene/entity.h"

#include <array>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <imgui.h>
#include "graphics/texture.h"
#include "imgui_internal.h"

namespace GV
{
  namespace UiUtils
  {
    void drawVec3Control(const std::string& label, glm::vec3& values, const glm::vec3& resetValues = {0.0f, 0.0f, 0.0f}, float columnWidth = 100.0f);

    template<typename T, typename UIFunction>
    void drawComponent(const std::string& name, GV::Entity& entity, UIFunction uiFunction)
    {	
      const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
      if (entity.hasComponent<T>())
      {
	auto& component = entity.getComponent<T>();
	ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImGui::Separator();
	bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
	ImGui::PopStyleVar();

	ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
	if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) ImGui::OpenPopup("ComponentSettings");
	bool removeComponent = false;
	if (ImGui::BeginPopup("ComponentSettings"))
	{
	  if (ImGui::MenuItem("Remove component")) removeComponent = true;
	  ImGui::EndPopup();
	}

	if (open)
	{
	  uiFunction(component);
	  ImGui::TreePop();
	}

	if (removeComponent) entity.removeComponent<T>();
      }
    }
    void drawComboBox(std::vector<std::string>& datasetName, int& currentIdx, const char* name);
    
    void helpMarker(const char* description);

    template<int T>
    void drawImageComboBox(std::array<std::unique_ptr<GV::Texture2D>, T>& images, std::array<std::string, T>& namesImages, int& currentIdx)
    {
      ImVec2 comboPos = ImGui::GetCursorScreenPos();
      if(ImGui::BeginCombo("Colormap", ""))
      {
	for (int i = 0; i < images.size(); i++)
	{
	  ImGui::GetWindowPos();
	  bool isSelected = (currentIdx == i);
	  ImGui::PushID(i);
	  if (ImGui::Selectable("", isSelected)) currentIdx = i;
	  ImGui::SameLine(0,0);
	  uint32_t textureID = images[i] -> getID();
	  float height = ImGui::GetTextLineHeight();
	  ImGui::Image(reinterpret_cast<ImTextureID>(textureID),ImVec2{10 * height, height}, ImVec2{0,0}, ImVec2{1,1});
	  ImGui::SameLine();
	  ImGui::Text(namesImages[i].c_str());
	  if (isSelected) ImGui::SetItemDefaultFocus();
	  ImGui::PopID();
	}
	ImGui::EndCombo();
      }
      // Reflect images selected in the ComboBox
      ImVec2 backupPos = ImGui::GetCursorScreenPos();
      ImGuiStyle& style = ImGui::GetStyle();
      ImGui::SetCursorScreenPos(ImVec2{comboPos.x + style.FramePadding.x, comboPos.y});
      float height = ImGui::GetTextLineHeight();
      uint32_t textureID = images[currentIdx] -> getID();
      ImGui::Image(reinterpret_cast<ImTextureID>(textureID),ImVec2{10 * height, height}, ImVec2{0,0}, ImVec2{1,1});
      ImGui::SameLine();
      ImGui::Text(namesImages[currentIdx].c_str());
      
      ImGui::SetCursorScreenPos(backupPos);
    }
  }
}
