#include "contentBrowserPanel.h"

#include "graphics/API.h"
#include "graphics/texture.h"
#include "imgui.h"
#include <filesystem>
#include <iostream>

constexpr static char* s_AssetsDirectory = "";

GV::ContentBrowserPanel::ContentBrowserPanel(GV::API api)
  : m_Api(api)
{
  m_DirectoryIcon = GV::Texture2D::create(m_Api, "resources/icons/directoryIcon.png");
  m_FileIcon = GV::Texture2D::create(m_Api, "resources/icons/fileIcon.png");

  m_BaseDirectory = "assets";
  m_CurrentDirectory = m_BaseDirectory;
}

void GV::ContentBrowserPanel::onImGuiRender()
{
  ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
  ImGui::Begin("Content Browser");
  // Left child
  ImGui::BeginChild("LeftContent", ImVec2(200, 0), true);
  ImGui::EndChild();
  


  // Right Panels
  ImGui::SameLine();
  ImGui::BeginChild("rightContent");

  if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
  {
    ImGui::SameLine();
    if (ImGui::Button("<-"))
    {
      m_CurrentDirectory = m_CurrentDirectory.parent_path();
    }
  }

  static float padding = 16.0f;
  static float thumbnailSize = 128.0f;
  float cellSize = thumbnailSize + padding;

  float panelWidth = ImGui::GetContentRegionAvail().x;
  int columnCount = (int)(panelWidth/cellSize);
  if (columnCount < 1) columnCount = 1;

  if (ImGui::BeginTable("table", columnCount))
    {
    //ImGui::TableNextRow();
    //ImGui::TableSetColumnIndex(0);

    for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
    {
      ImGui::TableNextColumn();
      const auto& path = directoryEntry.path();
      std::string filenameString = path.filename().string();
      ImGui::PushID(filenameString.c_str());


      GV::Texture2D* icon = directoryEntry.is_directory() ? m_DirectoryIcon.get() : m_FileIcon.get();
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

      ImGui::ImageButton((ImTextureID)(uint64_t)icon->getID(), ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 1), ImVec2(1, 0), 0, ImVec4 (0.0f, 0.0f, 0.0f, 0.0f));

      //ImGui::Text(filenameString.c_str());
      ImGui::PopStyleColor();

      if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
      {
	if (directoryEntry.is_directory()) m_CurrentDirectory /= path.filename();
      }

      ImGui::TextWrapped(filenameString.c_str());
      ImGui::PopID();
    }

    ImGui::EndTable();
  }

  ImGui::EndChild();
  ImGui::End();

}
