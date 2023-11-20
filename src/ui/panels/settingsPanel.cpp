
#include "settingsPanel.h"
#include "imgui.h"
#include "ui/editorLayer.h"

GV::SettingsPanel::SettingsPanel(GV::EditorLayer* editorLayer) 
  : m_EditorLayer(editorLayer) {}

void GV::SettingsPanel::onImGuiRender(bool *open)
{
  ImGui::Begin("Settings", open);
  // Left child
  static int selectedSetting = -1;
  ImGui::BeginChild("leftSetting", ImVec2(150,0), true);
  for (int i = 0; i < 1; i++)
  {
    if (ImGui::Selectable("Camera", selectedSetting == i)) 
    {
      selectedSetting = i;
    }
  }
  ImGui::EndChild();

  //Right child
  ImGui::SameLine();
  ImGui::BeginChild("rightSetting");
  if (selectedSetting == 0) showCameraSettings();
  ImGui::EndChild();
  ImGui::End();
}

void GV::SettingsPanel::showCameraSettings()
{
  //ImGui::SameLine();
  ImGui::SeparatorText("Speed");
  ImGui::InputFloat("Translation", &m_CameraController -> getCameraTranslationSpeed(), 0.1f, 1.0f, "%.1f");
  ImGui::InputFloat("Rotation", &m_CameraController -> getCameraRotationSpeed(), 0.1f, 1.0f, "%.1f");
  
  ImGui::Dummy({0.0f, 20.0f});
  ImGui::SeparatorText("View Frustum");
  if (ImGui::InputFloat("Near Clip", &m_CameraController -> getCameraNearClip(), 0.1f, 1.0f, "%.1f")) m_CameraController-> updateCameraProjectionMatrix();
  if (ImGui::InputFloat("Far Clip", &m_CameraController -> getCameraFarClip(), 1.0f, 1.0f, "%.f")) m_CameraController -> updateCameraProjectionMatrix();

}
